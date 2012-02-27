#include "Scene.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <iomanip>
#include <string>
#include <streambuf>

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

