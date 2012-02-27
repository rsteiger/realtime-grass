////////////////////////////////////////////////////////////////
//                                                            //
// glutMaster.c++                                             //
// beta version 0.3 - 9/9/97)                                 //
//                                                            //
// George Stetten and Korin Crawford                          //
// copyright given to the public domain                       //
//                                                            //
// Please email comments to email@stetten.com                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "glutMaster.h"
#include "glutWindow.h"
#include <string>
                                                       
GlutWindow *viewPorts[MAX_NUMBER_OF_WINDOWS]; 

int GlutMaster::currentIdleWindow   = 0;
int GlutMaster::idleFunctionEnabled = 0;


GlutMaster::GlutMaster(){

   // Create dummy variables 

   char *dummy_argv[1];
   int dummy_argc = 1;

   std::string str("run");
   char * writable = new char[str.size() + 1];
   std::copy(str.begin(), str.end(), writable);
   writable[str.size()] = '\0'; // don't forget the terminating 0
   dummy_argv[0] = writable;

   // Initialize GLUT
   glutInit(&dummy_argc, dummy_argv);

   // don't forget to free the string after finished using it
   delete[] writable;

}

GlutMaster::~GlutMaster(){

}
 
void GlutMaster::CallBackDisplayFunc(void){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackDisplayFunc();
}

void GlutMaster::CallBackIdleFunc(void){

   if(idleFunctionEnabled && currentIdleWindow){
      glutSetWindow(currentIdleWindow);
      viewPorts[currentIdleWindow]->CallBackIdleFunc();
   }
}
 
void GlutMaster::CallBackKeyboardFunc(unsigned char key, int x, int y){
   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackKeyboardFunc(key, x, y);
}
 
void GlutMaster::CallBackKeyboardUpFunc(unsigned char key, int x, int y){
   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackKeyboardUpFunc(key, x, y);
}

void GlutMaster::CallBackMotionFunc(int x, int y){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackMotionFunc(x, y);
}

void GlutMaster::CallBackMouseFunc(int button, int state, int x, int y){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackMouseFunc(button, state, x, y);
}

void GlutMaster::CallBackPassiveMotionFunc(int x, int y){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackPassiveMotionFunc(x, y);
}

void GlutMaster::CallBackReshapeFunc(int w, int h){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackReshapeFunc(w, h);
}

void GlutMaster::CallBackSpecialFunc(int key, int x, int y){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackSpecialFunc(key, x, y);
}   

void GlutMaster::CallBackVisibilityFunc(int visible){

   int windowID = glutGetWindow();
   viewPorts[windowID]->CallBackVisibilityFunc(visible);
}

void GlutMaster::CallGlutCreateWindow(std::string setTitle, GlutWindow &glutWindow){

   // Open new window, record its windowID , 
   char * writable = new char[setTitle.size() + 1];
   std::copy(setTitle.begin(), setTitle.end(), writable);
   writable[setTitle.size()] = '\0'; // don't forget the terminating 0

   int windowID = glutCreateWindow(writable);

   // don't forget to free the string after finished using it
   delete[] writable;



   glutWindow.SetWindowID(windowID);

   // Store the address of new window in global array 
   // so GlutMaster can send events to propoer callback functions.

   viewPorts[windowID] = &glutWindow;

   // Hand address of universal static callback functions to Glut.
   // This must be for each new window, even though the address are constant.

   glutDisplayFunc(CallBackDisplayFunc);
   glutIdleFunc(CallBackIdleFunc); 
   glutKeyboardFunc(CallBackKeyboardFunc);
   glutKeyboardUpFunc(CallBackKeyboardUpFunc);
   glutSpecialFunc(CallBackSpecialFunc);
   glutMouseFunc(CallBackMouseFunc);
   glutMotionFunc(CallBackMotionFunc);
   glutPassiveMotionFunc(CallBackPassiveMotionFunc);
   glutReshapeFunc(CallBackReshapeFunc); 
   glutVisibilityFunc(CallBackVisibilityFunc);
}

void GlutMaster::CallGlutMainLoop(void){

   glutMainLoop();
}
                              
void GlutMaster::DisableIdleFunction(void){

   idleFunctionEnabled = 0;
}

void GlutMaster::EnableIdleFunction(void){

   idleFunctionEnabled = 1;
}

int GlutMaster::IdleFunctionEnabled(void){

   // Is idle function enabled?

   return(idleFunctionEnabled);
}

int GlutMaster::IdleSetToCurrentWindow(void){

   // Is current idle window same as current window?

   return( currentIdleWindow == glutGetWindow() );
}

void GlutMaster::SetIdleToCurrentWindow(void){

   currentIdleWindow = glutGetWindow();
}














