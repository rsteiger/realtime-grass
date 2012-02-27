/*
 *  CPE 471 lab 4 - draws a partial cube using a VBO and IBO 
 *  glut/OpenGL/GLSL application   
 *  Uses glm to handle matrix transforms for a view matrix, projection matrix and
 *  model transform matrix
 *
 *  Created by zwood on 1/17/12 
 *  Copyright 2010 Cal Poly. All rights reserved.
 *
 *****************************************************************************/
#include "glutMaster/glutMaster.h"
#include "cameraWindow.h"
#include "Shader.h"
#include "Scene.h"

#include <stdlib.h>
#include <stdio.h>
#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc

using namespace std;

/* intialize ground data */
static Model *initGround(float groundSize, float groundY) {
   Model *groundModel = new Model();
   Mesh ground("ground.m");
   groundModel->meshes.push_back(ground);

   Instance gInst;
   glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0,groundY,0) );
   gInst.transform = glm::scale(Trans, glm::vec3(groundSize,groundSize,groundSize));
   groundModel->instances.push_back(gInst);

   return groundModel;
}

float rf() {
   return (float)rand()/(float)RAND_MAX;
}

/* intialize the cube data */
static Model *initCube() {
   Model *cubeModel = new Model();
   Mesh cube("cube.m");
   cubeModel->meshes.push_back(cube);

   srand(0);
   float prev_width = 1.5;
   float prev_length = 1.5;
   float height = rf() + 1;
   Instance cInst;
   glm::vec3 size(prev_length,height,prev_width);
   glm::vec3 pos(-5.0f+prev_width/2,height/2.0,-5.0f+prev_length/2);
   glm::mat4 Trans = glm::translate(glm::mat4(1.0f), pos+glm::vec3(0.0,-0.2,0) );
   cInst.transform = glm::scale(Trans, size);
   cubeModel->instances.push_back(cInst);

   for ( int i = 0; i < 10; i++) {
      float width = rf()+0.5;
      height = rf() + 0.5;
      glm::vec3 pos((float)(-5 + prev_width + width/2),height/2,-5);
      glm::vec3 size(width,height,1);
      glm::mat4 Trans = glm::translate(glm::mat4(1.0f), pos+glm::vec3(0.0,-0.2,0) );
      cInst.transform = glm::scale(Trans, size);
      prev_width += width;
      cubeModel->instances.push_back(cInst);
   }

   for ( int i = 0; i < 10; i++) {
      float length = rf()+0.5;
      height = rf() + 0.5;
      glm::vec3 pos((float)-5,height/2,(-5 + prev_length + length/2));
      glm::vec3 size(1.0,height,length);
      glm::mat4 Trans = glm::translate(glm::mat4(1.0f), pos+glm::vec3(0.0,-0.2,0) );
      cInst.transform = glm::scale(Trans, size);
      prev_length += length;
      cubeModel->instances.push_back(cInst);
   }

   return cubeModel;
}

void InitGeom(Scene &s) {
   Model *ground = initGround(20.0, -0.2);
   Model *cube = initCube();

   Model *t = new Model();
   Mesh tyra("tyra_1k.m");
   t->meshes.push_back(tyra);
   for (int i = 1; i < 5; i ++) {
      glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3((float)i,0,0));
      t->instances.push_back(Instance(trans));
   }

   Model *bunny = new Model();
   Mesh b("bunny.m");
   bunny->meshes.push_back(b);
   for (int i = 1; i < 2; i ++) {
      glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3((float)i,0,2.0));
      bunny->instances.push_back(Instance(trans));
   }


   s.models.push_back(*ground);
   s.models.push_back(*cube);
   s.models.push_back(*t);
   s.models.push_back(*bunny);

   s.lights.push_back(Light(0,1,1));
}


class MyAction : public KeyAction {
   public:
   virtual void operator()(Scene scene, double deltaT) { 
      cout << "MyAction: " << deltaT << endl; 
   };
};


void addKeyResponders(CameraWindow &cw) {
   int moveSensitivity = 10;

   // Up is pressed
   struct UpAction : public KeyAction {
      Camera &cam;
      void operator()(double deltaT) {
         float px = cos(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         float py = sin(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         cam.trans += glm::vec3(-py,0,px);
      }
      UpAction(Camera &cam) : cam(cam) { }
   };
   KeyAction *up = new UpAction(cw.camera);
   cw.addKeyHandler('w',up);
   cw.addKeyHandler(',',up);

   // Left is pressed
   struct LeftAction : public KeyAction {
      Camera &cam;
      void operator()(double deltaT) {
         float px = cos(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         float py = sin(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         cam.trans += glm::vec3(px,0,py);
      }
      LeftAction(Camera &cam) : cam(cam) { }
   };
   KeyAction *left = new LeftAction(cw.camera);
   cw.addKeyHandler('a',left);

   // Down is pressed
   struct DownAction : public KeyAction {
      Camera &cam;
      void operator()(double deltaT) {
         float px = cos(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         float py = sin(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         cam.trans += glm::vec3(py,0,-px);
      }
      DownAction(Camera &cam) : cam(cam) { }
   };
   KeyAction *down = new DownAction(cw.camera);
   cw.addKeyHandler('s',down);
   cw.addKeyHandler('o',down);

   // Right is pressed
   struct RightAction : public KeyAction {
      Camera &cam;
      void operator()(double deltaT) {
         float px = cos(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         float py = sin(3.14159*cam.rot/180) * deltaT * cam.moveSensitivity;
         cam.trans += glm::vec3(-px,0,-py);
      }
      RightAction(Camera &cam) : cam(cam) { }
   };
   KeyAction *right = new RightAction(cw.camera);
   cw.addKeyHandler('d',right);
   cw.addKeyHandler('e',right);
}

void removeKeyResponders(CameraWindow &cw) {
   delete cw.removeKeyHandler('w');
   delete cw.removeKeyHandler('a');
   delete cw.removeKeyHandler('s');
   delete cw.removeKeyHandler('d');
   cw.removeKeyHandler(',');
   cw.removeKeyHandler('o');
   cw.removeKeyHandler('e');
}

int main( int argc, char *argv[] ) {
   GlutMaster glutMaster;

   CameraWindow window(glutMaster, 500, 500, 200, 400, "Second Window");
 
	glClearColor (1.0f, 1.0f, 1.0f, 1.0f);								
 	glClearDepth (1.0f);
 	glDepthFunc (GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   addKeyResponders(window);
 
   Scene sc;
   InitGeom(sc);   
   window.scene = &sc;

   //test the openGL version
   getGLversion();

   //install the shader
   Shader shader(textFileRead((char *)"shader.vert"), 
                 textFileRead((char *)"shader.geom"),
                 textFileRead((char *)"shader.frag"));
   window.shader = shader;

   window.StartSpinning(glutMaster);

   //glutMaster.CallGlutMainLoop();
   glutMainLoop();

   removeKeyResponders(window);
   return 0;
}
