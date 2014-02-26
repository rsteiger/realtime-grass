#include "cameraWindow.h"
#include <string>
#include <vector>
#include <math.h>
#include <curses.h>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr


#define FRAMERATE

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
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);


  glutMaster.CallGlutCreateWindow(string(title), *this);

  time = glutGet(GLUT_ELAPSED_TIME);

#ifdef FRAMERATE
  initscr();        /* Start curses mode         */
#endif
}

CameraWindow::~CameraWindow(){
#ifdef FRAMERATE
  endwin();
#endif
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
    scene->tileFrustum(camera);

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
  int newTime = glutGet(GLUT_ELAPSED_TIME);
  int deltaT = newTime - time;
  time = newTime;

  vector<KeyHandler>::iterator it;
  for (it = keyHandlers.begin(); it != keyHandlers.end(); it++) {
    if (keyStates[it->key])
      (*it->action)(deltaT);
  }


#ifdef FRAMERATE
  static int frames = 0;
  static int deltaSum = 0;
  deltaSum += deltaT;
  frames++;
  if (deltaSum > 1000) {
    float fps = 1000 * (float)frames / (float)deltaSum;
    printw("FPS: %4.2f",fps); /* Print Hello World      */
    move(0,0);
    refresh();        /* Print it on to the real screen */
    frames = 0;
    deltaSum = 0;
  }
#endif

  CallBackDisplayFunc();
}

void CameraWindow::StartSpinning(GlutMaster &glutMaster) {
  glutMaster.SetIdleToCurrentWindow();
  glutMaster.EnableIdleFunction();
}

void DrawModel(PhongShader &shader, Model &model) {
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
      glUniform3f(shader.color, 0.3f, 0.2f, 0.0f);
      glDrawElements(GL_TRIANGLES, mi->iboLen, GL_UNSIGNED_SHORT, 0);
    }
  }

  safe_glDisableVertexAttribArray(shader.position);
  safe_glDisableVertexAttribArray(shader.normal);
}

void CameraWindow::DrawGrass(GrassShader &shader, Grass &grass) {
  safe_glEnableVertexAttribArray(shader.position);
  safe_glEnableVertexAttribArray(shader.direction);

  glUniform1f(shader.time,(float)time/1000.0f);
  glUniform1i(shader.texture, 0);
  glUniform1i(shader.alpha, 1);
  glUniform1i(shader.noise, 2);

  glBindBuffer(GL_ARRAY_BUFFER, grass.dirBuffObj);
  safe_glVertexAttribPointer(shader.direction, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, grass.buffObj);
  safe_glVertexAttribPointer(shader.position, 4, GL_FLOAT, GL_FALSE, 0, 0);

  vector<Instance> objects = grass.instances;
  vector<Instance>::iterator it;
  for (it = objects.begin(); it != objects.end(); it++) {
    safe_glUniformMatrix4fv(shader.modelMatrix, glm::value_ptr((*it).transform));
    glDrawArrays(GL_POINTS, 0, grass.verts);
  }

  safe_glDisableVertexAttribArray(shader.position);
  safe_glDisableVertexAttribArray(shader.direction);
}

void CameraWindow::CallBackDisplayFunc(void){
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 CamRotateY = glm::rotate(glm::mat4(1.0f), camera.pitch, glm::vec3(1.0f,0,0));
  glm::mat4 CamRotateX = glm::rotate(CamRotateY, camera.rot, glm::vec3(0.0f, 1, 0));
  glm::mat4 CamTrans = glm::translate(CamRotateX, camera.trans);
  glm::mat4 Projection = glm::perspective((float)camera.viewangle, (float)width/height, 0.1f, 100.f);


  // Draw the grass
  glDepthFunc (GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glUseProgram(grassShader->program);
  safe_glUniformMatrix4fv(grassShader->projMatrix, glm::value_ptr(Projection));
  safe_glUniformMatrix4fv(grassShader->viewMatrix, glm::value_ptr(CamTrans));
  glUniform1i(grassShader->numLights,scene->lights.size());
  glUniform3fv(grassShader->lights,scene->lights.size(),(GLfloat*)&scene->lights[0]);

  DrawGrass(*grassShader, scene->grass);


  glutSwapBuffers();
}

void CameraWindow::CallBackKeyboardFunc(unsigned char key, int x, int y) {
  keyStates[key] = true;
  switch(key) {
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
  this->camera.aspectRatio = (float)w/h;
  glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));
}


KeyAction *CameraWindow::removeKeyHandler(unsigned char key) {
  vector<KeyHandler>::iterator it = keyHandlers.begin();
  while(it != keyHandlers.end() && it->key != key)
    it++;
  return it->action;
}

