#include "Scene.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <iomanip>
#include <string>
#include <streambuf>
#include <set>
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc

using namespace std;

namespace MeshLoader {
   class Vector {
      public:
         union {
            float x;
            float r;
         };
         union {
            float y;
            float g;
         };
         union {
            float z;
            float b;
         };
         Vector() : x(0), y(0), z(0) {};
         Vector(float mx, float my, float mz) : x(mx), y(my), z(mz) {};

         Vector operator-(const Vector &v) const {
            return Vector(x - v.x, y - v.y, z - v.z);
         }

         Vector &operator+=(const Vector &v) {
            x+=v.x, y+=v.y, z+=v.z;
            return *this;
         }

         float dot(const Vector &v) const {
            return x * v.x + y * v.y + z * v.z;
         }

         Vector cross(const Vector &v) const {
            return Vector(y*v.z - v.y*z, v.x*z - x*v.z, x*v.y - v.x*y);
         }

         Vector &normalize() {
            float magnitude = sqrt(this->dot(*this));
            x /= magnitude, y /= magnitude, z /= magnitude;
            return *this;
         }
   };

   class Vertex {
      public:
         Vector position;
         explicit Vertex(stringstream &stream);
   };

   class Face {
      public:
         unsigned short vertexNdx[3];
         Face(stringstream &stream, const vector<Vertex> &vertices);
   };
};

using namespace MeshLoader;

Mesh::Mesh(const std::string &filename) {
   ifstream file;
   file.open(filename.c_str());

   if (!file.is_open()) {
      cerr << "Unable to open mesh file: " << filename << endl;
      return;
   }

   vector<Vertex> vertices;
   vector<Vector> normals;
   vector<Face> faces;

   while (file) {
      string readLine;
      getline(file, readLine);
      stringstream stream(readLine);

      string label;
      stream >> label;

      if (label.find("#") != string::npos) {
         continue; // We read a comment line
      } else if (label == "Vertex") {
         vertices.push_back(Vertex(stream)); 
         normals.push_back(Vector());
      } else if (label == "Face") {
         faces.push_back(Face(stream,vertices));
         Face &f = faces.back();
         Vector verts[3] = {vertices[f.vertexNdx[0]].position, 
            vertices[f.vertexNdx[1]].position, 
            vertices[f.vertexNdx[2]].position};
         Vector normal = (verts[0] - verts[1]).cross(verts[0] - verts[2]);
         normals[f.vertexNdx[0]] += normal;
         normals[f.vertexNdx[1]] += normal;
         normals[f.vertexNdx[2]] += normal;
      } else if (label == "") {
         // New line
      } else if (label == "Corner") {
         // Not doing any normal calculations
      } else {
         cerr << "Expected 'Vertex' or 'Face', encountered '" << label << "'." << endl;
      }
   }

   vector<Vector>::iterator it;
   for (it = normals.begin(); it != normals.end(); it++) {
      it->normalize();
   }

   iboLen = faces.size() * 3;
   glGenBuffers(1, &buffObj);
   glBindBuffer(GL_ARRAY_BUFFER, buffObj);
   glBufferData(GL_ARRAY_BUFFER,sizeof(Vector)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

   glGenBuffers(1, &indxBuffObj);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indxBuffObj);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboLen*sizeof(short),&faces[0], GL_STATIC_DRAW);

   glGenBuffers(1, &normalBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, normalBuffObj);
   glBufferData(GL_ARRAY_BUFFER,sizeof(Vector)*normals.size(),&normals[0], GL_STATIC_DRAW);

}

Face::Face(stringstream &stream, const vector<Vertex> &vertices) {
   int index;
   stream >> index >> vertexNdx[0] >> vertexNdx[1] >> vertexNdx[2];
   vertexNdx[0]--;
   vertexNdx[1]--;
   vertexNdx[2]--;
   // Color logic would go here
}

Vertex::Vertex(stringstream &stream) {
   int index;
   stream >> index  >> position.x >> position.y >> position.z;
}

struct ScannedResult {
   ScannedResult(int X, int Y) : x(X), y(Y) {  }
   bool operator<(ScannedResult const& rhs) const { return (y<rhs.y) || ( (y==rhs.y) && (x<rhs.x) ); }
   int x, y;
};

static bool swap_xy = false, flip_y = false;  

void scanLineSegment(int x1, int y1, int x2, int y2, set<ScannedResult> &output) {
   assert( x2-x1 >= 0 && x2-x1 >= y2-y1 && y2-y1 >= 0);

   bool horizontal = y1==y2, diagonal = y2-y1==x2-x1;
   float scale = 1.0 / (x2 - x1);

   int y = y1; 
   float yy = y1, y_step = (y2 - y1) * scale;

   for (int x = x1; x <= x2; ++x) {
      int X(x), Y(y);
      if (swap_xy) swap(X,Y);
      if (flip_y)  Y = -Y; 

      output.insert( ScannedResult(X, Y) );

      if (!horizontal) {
         if (diagonal) y++;
         else y = int(round( yy += y_step ));
      }
   }
}

void ScanLineSegment(int x1, int y1, int x2, int y2, set<ScannedResult> &output) {
   if(x1 > x2) {                                       // always scan convert from left to right.
      swap(x1, x2); swap(y1, y2); 
   }
   if((flip_y = y1 > y2)) {                         // always scan convert from down to up.
      y1 = -y1, y2 = -y2;
   }
   if((swap_xy = y2-y1 > x2-x1)) {                   // and always scan convert a line with <= 45 deg to x-axis.
      swap(x1, y1), swap(x2, y2);
   }
   scanLineSegment(x1, y1,  x2, y2, output);
}

void Scene::fillTriangle(int x1, int y1,
                  int x2, int y2,
                  int x3, int y3) {
   set<ScannedResult> triangle_pnts;                 // sorted in y val, and in x val if y val the same.
   ScanLineSegment(x1, y1,  x2, y2, triangle_pnts);
   ScanLineSegment(x2, y2,  x3, y3, triangle_pnts);
   ScanLineSegment(x3, y3,  x1, y1,  triangle_pnts);

   bool curvalset = false;
   int cur_yval = 0;
   set<ScannedResult> same_yval;
   set<ScannedResult> scanned_pnts;
   set<ScannedResult> allPositions;

   for (set<ScannedResult>::iterator it = triangle_pnts.begin(); it != triangle_pnts.end(); ++it) {
      int y = it->y;
      if (!curvalset || y != cur_yval) {
         if (same_yval.size()) {
            set<ScannedResult>::iterator it1 = same_yval.begin(), it2 = --same_yval.end();
            ScanLineSegment(it1->x, cur_yval, it2->x, cur_yval, scanned_pnts);
            same_yval.clear();
         } 
         cur_yval = y; 
         curvalset = true;
      }
      same_yval.insert(*it);
   }

   for (set<ScannedResult>::iterator it = scanned_pnts.begin(); it != scanned_pnts.end(); ++it) {
      allPositions.insert(ScannedResult(it->x,it->y));
      allPositions.insert(ScannedResult(it->x,it->y-1));
      allPositions.insert(ScannedResult(it->x-1,it->y-1));
      allPositions.insert(ScannedResult(it->x-1,it->y));
   }

   for (set<ScannedResult>::iterator it = allPositions.begin(); it != allPositions.end(); ++it) {
      glm::mat4 position = glm::translate(glm::mat4(1.0), glm::vec3(it->x,0,it->y));
      grass.instances.push_back(position);
   }
}

#define PI 3.14159
void Scene::tileFrustum(Camera &camera) {
   const int distance = 20;
   grass.instances = vector<Instance>();

   float ar = camera.aspectRatio;
   float theta = PI*(camera.viewangle/2)/180;
   float phi = atan(2*ar*tan(theta));
   float cr = PI*(camera.rot)/180;

   fillTriangle((int)-(camera.trans.x),
                (int)-(camera.trans.z),
                (int)-(camera.trans.x + distance * -sin(cr+phi)),
                (int)-(camera.trans.z + distance * cos(cr+phi)),
                (int)-(camera.trans.x + distance * -sin(cr-phi)),
                (int)-(camera.trans.z + distance * cos(cr-phi)));
}

Grass::Grass() {
   const static int density = 8;
   float vertices[density*density][4];
   float dirs[density*density];

   srand((unsigned)time(0));

   for (int i = 0; i < density; i++ ) {
      for (int j = 0; j < density; j++ ) {
         vertices[density*i+j][0] = (float)i/density + ((float)rand()/(float)RAND_MAX - .5)/density;
         vertices[density*i+j][1] = 0;
         vertices[density*i+j][2] = (float)j/density + ((float)rand()/(float)RAND_MAX - .5)/density;
         vertices[density*i+j][3] = 1;
         dirs[density*i+j] = 3.14159 * (float)rand()/(float)RAND_MAX;
      }
   }
   verts = density*density;

   glGenBuffers(1, &buffObj);
   glBindBuffer(GL_ARRAY_BUFFER, buffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glGenBuffers(1, &dirBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, dirBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(dirs), dirs, GL_STATIC_DRAW);
}
