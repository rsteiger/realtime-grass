#include "glutMaster/glutMaster.h"
#include "GLSL_helper.h"

#include "cameraWindow.h"
#include "Shader.h"
#include "Scene.h"

#include <cstdlib>
#include <cstdio>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc

using namespace std;

/* intialize ground data */
static Model *initGround(float groundSize, float groundY) {
   Model *groundModel = new Model();
   Mesh ground("bunny.m");
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
void InitGeom(Scene &s) {
   Model *ground = initGround(100.0, -10);
   s.models.push_back(*ground);

   s.lights.push_back(Light(0,20,1));
}


class MyAction : public KeyAction {
   public:
   virtual void operator()(Scene scene, double deltaT) { 
      cout << "MyAction: " << deltaT << endl; 
   };
};

float pxf(Camera &cam, int deltaT) {
   return cos(3.14159*cam.rot/180) * (float)deltaT/1000 * cam.moveSensitivity;
}
float pyf(Camera &cam, int deltaT) {
   return sin(3.14159*cam.rot/180) * (float)deltaT/1000 * cam.moveSensitivity;
}

void addKeyResponders(CameraWindow &cw) {
   int moveSensitivity = 10;

   // Up is pressed
   struct UpAction : public KeyAction {
      Camera &cam;
      Scene &s;
      void operator()(int deltaT) {
         float px = pxf(cam,deltaT), py = pyf(cam,deltaT);
         cam.trans += glm::vec3(-py,0,px);
         s.tileFrustum(cam);
      }
      UpAction(Camera &cam, Scene &scene) : cam(cam), s(scene) { }
   };
   KeyAction *up = new UpAction(cw.camera,*cw.scene);
   cw.addKeyHandler('w',up);
   cw.addKeyHandler(',',up);

   // Left is pressed
   struct LeftAction : public KeyAction {
      Camera &cam;
      Scene &s;
      void operator()(int deltaT) {
         float px = pxf(cam,deltaT), py = pyf(cam,deltaT);
         cam.trans += glm::vec3(px,0,py);
         s.tileFrustum(cam);
      }
      LeftAction(Camera &cam, Scene &scene) : cam(cam), s(scene) { }
   };
   KeyAction *left = new LeftAction(cw.camera,*cw.scene);
   cw.addKeyHandler('a',left);

   // Down is pressed
   struct DownAction : public KeyAction {
      Camera &cam;
      Scene &s;
      void operator()(int deltaT) {
         float px = pxf(cam,deltaT), py = pyf(cam,deltaT);
         cam.trans += glm::vec3(py,0,-px);
         s.tileFrustum(cam);
      }
      DownAction(Camera &cam, Scene &scene) : cam(cam), s(scene) { }
   };
   KeyAction *down = new DownAction(cw.camera,*cw.scene);
   cw.addKeyHandler('s',down);
   cw.addKeyHandler('o',down);

   // Right is pressed
   struct RightAction : public KeyAction {
      Camera &cam;
      Scene &s;
      void operator()(int deltaT) {
         float px = pxf(cam,deltaT), py = pyf(cam,deltaT);
         cam.trans += glm::vec3(-px,0,-py);
         s.tileFrustum(cam);
      }
      RightAction(Camera &cam, Scene &scene) : cam(cam), s(scene) { }
   };
   KeyAction *right = new RightAction(cw.camera,*cw.scene);
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

   CameraWindow window(glutMaster, 900, 900, 100, 100, "Grass Demo");
   window.camera.trans -= glm::vec3(0.0f,1.5f,0.0f);

#ifndef __APPLE__
   glewInit();
#endif

   Scene sc;
   InitGeom(sc);
   window.scene = &sc;
   sc.tileFrustum(window.camera);

   addKeyResponders(window);

   //test the openGL version
   getGLversion();

   window.grassShader = new GrassShader();
   window.phongShader = new PhongShader();

   window.StartSpinning(glutMaster);

   //glutMaster.CallGlutMainLoop();
   glutMainLoop();

   removeKeyResponders(window);
   return 0;
}
