////////////////////////////////////////////////////////////////
//                                                            //
// glutMaster.h                                               //
// beta version 0.3 - 9/9/97)                                 //
//                                                            //
// George Stetten and Korin Crawford                          //
// copyright given to the public domain                       //
//                                                            //
// Please email comments to email@stetten.com                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef __GLUT_MASTER_H__
#define __GLUT_MASTER_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "glutWindow.h"

#include <string>

#define MAX_NUMBER_OF_WINDOWS 256

class GlutMaster{

private:

   static void CallBackDisplayFunc(void);
   static void CallBackIdleFunc(void);
   static void CallBackKeyboardFunc(unsigned char key, int x, int y);
   static void CallBackKeyboardUpFunc(unsigned char key, int x, int y);
   static void CallBackMotionFunc(int x, int y);
   static void CallBackMouseFunc(int button, int state, int x, int y);
   static void CallBackPassiveMotionFunc(int x, int y);
   static void CallBackReshapeFunc(int w, int h);
   static void CallBackSpecialFunc(int key, int x, int y);
   static void CallBackVisibilityFunc(int visible);

   static int currentIdleWindow;
   static int idleFunctionEnabled;

public:
   GlutMaster();
   ~GlutMaster();

   void  CallGlutCreateWindow(std::string setTitle, GlutWindow &glutWindow);
   void  CallGlutMainLoop(void);

   void  DisableIdleFunction(void);
   void  EnableIdleFunction(void);
   int   IdleFunctionEnabled(void);

   int   IdleSetToCurrentWindow(void);
   void  SetIdleToCurrentWindow(void);
};

#endif
