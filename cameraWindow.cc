#include "cameraWindow.h"
#include <string>
#include <vector>
#include <math.h>

#include <ApplicationServices/ApplicationServices.h>

#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

using namespace std;

CameraWindow::CameraWindow(GlutMaster &glutMaster,
                       int setWidth, int setHeight,
                       int setInitPositionX, int setInitPositionY,
                       const char *title) : keyStates(), scene() {
   width  = setWidth;               
   height = setHeight;

   initPositionX = setInitPositionX;
   initPositionY = setInitPositionY;

   this->camera.rot = 0;
   this->camEnabled = false;

   glutInitWindowSize(width, height);
   glutInitWindowPosition(initPositionX, initPositionY);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

   glutMaster.CallGlutCreateWindow(string(title), *this);
}

CameraWindow::~CameraWindow(){
   glutDestroyWindow(windowID);
}

void CameraWindow::CallBackPassiveMotionFunc(int x, int y) {
   int deltaX = x - width/2;
   int deltaY = y - height/2;

   #ifdef __APPLE__
   CGGetLastMouseDelta(&deltaX,&deltaY);
   #endif
 
   if (this->camEnabled) {
      #ifndef __APPLE__
      glutWarpPointer(startx, starty);
      #else
      CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, 
       CGPointMake(glutGet(GLUT_WINDOW_X)+startx, 
       glutGet(GLUT_WINDOW_Y) + starty));
      #endif

      camera.rot += (float)deltaX / 10 * camera.sensitivity;
      camera.pitch += (float)deltaY / 10 * camera.sensitivity;
      if ( camera.pitch > 50 ) camera.pitch = 50;
      if ( camera.pitch < -50) camera.pitch = -50;

      glutPostRedisplay();
   }
}

void CameraWindow::CallBackMouseFunc(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON) {
      if (state == GLUT_DOWN) { /* if the left button is clicked */
         if (camEnabled) {
            DisableView();
         } else {
            EnableView(x,y);
         }
      }
   } 
}

void CameraWindow::DisableView() {
   this->camEnabled = false;
   glutSetCursor(GLUT_CURSOR_INHERIT);  
   #ifdef __APPLE__
   CGDisplayShowCursor(kCGDirectMainDisplay);
   CGAssociateMouseAndMouseCursorPosition(true);
   #endif
}


void CameraWindow::EnableView(int x, int y) {
   startx = x;
   starty = y;
   this->camEnabled = true;
   glutSetCursor(GLUT_CURSOR_NONE);  
   #ifdef __APPLE__
   CGDisplayHideCursor(kCGDirectMainDisplay);
   CGAssociateMouseAndMouseCursorPosition(false); 
   #endif
}

void CameraWindow::CallBackIdleFunc(void) {
   clock_t newTime = clock();
   double deltaT = (double)(newTime - time) / CLOCKS_PER_SEC;
   time = newTime; 
 
   vector<KeyHandler>::iterator it;
   for (it = keyHandlers.begin(); it != keyHandlers.end(); it++) {
      if (keyStates[it->key])
         (*it->action)(deltaT); 
   }
   CallBackDisplayFunc();
}

void CameraWindow::StartSpinning(GlutMaster &glutMaster) {
   glutMaster.SetIdleToCurrentWindow();
   glutMaster.EnableIdleFunction();
}

void DrawModel(Shader &shader, Model &model) {
   safe_glEnableVertexAttribArray(shader.position);
   safe_glEnableVertexAttribArray(shader.normal);

   vector<Mesh> meshes = model.meshes;
   vector<Mesh>::iterator mi;
   for (mi = meshes.begin(); mi != meshes.end(); mi++) {

      glBindBuffer(GL_ARRAY_BUFFER, mi->buffObj);
      safe_glVertexAttribPointer(shader.position, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mi->indxBuffObj);

      glBindBuffer(GL_ARRAY_BUFFER, mi->normalBuffObj);
      safe_glVertexAttribPointer(shader.normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

      vector<Instance> objects = model.instances;
      vector<Instance>::iterator it;
      for (it = objects.begin(); it != objects.end(); it++) {
         safe_glUniformMatrix4fv(shader.modelMatrix, glm::value_ptr((*it).transform));
         glUniform3f(shader.color, 0.7f, 0.5f, 0.8f);
         glDrawElements(GL_POINTS, mi->iboLen, GL_UNSIGNED_SHORT, 0);
      }
   }

   safe_glDisableVertexAttribArray(shader.position);
   safe_glDisableVertexAttribArray(shader.normal);
}

void CameraWindow::CallBackDisplayFunc(void){
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				
   //Start our shader	
   glUseProgram(shader.program);

   /* only set the projection and view matrix once */
   glm::mat4 Projection = glm::perspective(80.0f, (float)width/height, 0.1f, 100.f);	
   safe_glUniformMatrix4fv(shader.projMatrix, glm::value_ptr(Projection));

   glm::mat4 CamRotateY = glm::rotate(glm::mat4(1.0f), camera.pitch, glm::vec3(1.0f,0,0));
   glm::mat4 CamRotateX = glm::rotate(CamRotateY, camera.rot, glm::vec3(0.0f, 1, 0));
   glm::mat4 CamTrans = glm::translate(CamRotateX, camera.trans);
   safe_glUniformMatrix4fv(shader.viewMatrix, glm::value_ptr(CamTrans));

   glUniform1i(shader.numLights,scene->lights.size());
   glUniform3fv(shader.lights,scene->lights.size(),(GLfloat*)&scene->lights[0]);

   /* make sure model transforms for the ground plane are identity */ 
   vector<Model> models = scene->models;
   vector<Model>::iterator it;
   for (it = models.begin(); it < models.end(); it++) {
      DrawModel(shader,*it);
   }

   //Diisable the shader
   glUseProgram(0);	
   glutSwapBuffers();
}

void CameraWindow::CallBackKeyboardFunc(unsigned char key, int x, int y) {
   keyStates[key] = true;   
   switch( key ) {
      case 27:
         DisableView();
         break;
      case 'q': case 'Q' :
         exit( EXIT_SUCCESS );
         break;
   }

   glutPostRedisplay();
}

void CameraWindow::CallBackKeyboardUpFunc(unsigned char key, int x, int y) {
   keyStates[key] = false;
}

void CameraWindow::CallBackReshapeFunc(int w, int h){
   width = w;
   height= h;
   glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				
}


KeyAction *CameraWindow::removeKeyHandler(unsigned char key) {
   vector<KeyHandler>::iterator it = keyHandlers.begin();
   while(it != keyHandlers.end() && it->key != key)
      it++;
   return it->action;
}

