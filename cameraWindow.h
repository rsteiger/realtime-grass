#ifndef __CAMERA_WINDOW_H__
#define __CAMERA_WINDOW_H__

#include "glutMaster/glutMaster.h"
#include "Shader.h"
#include "Scene.h"
#include <time.h>
#include <vector>

class KeyAction {
   public:
   virtual void operator()(double deltaT) { };
};

class KeyHandler {
   public:
      KeyAction *action;
      unsigned char key; 
      KeyHandler(KeyAction *a, unsigned char k) : action(a), key(k) { };
};


class CameraWindow : public GlutWindow{
   bool                       camEnabled;
   int                        height, width;
   int                        initPositionX, initPositionY;
   int                        startx, starty;
   bool                       keyStates[256];
   clock_t                    time;
   std::vector<KeyHandler>    keyHandlers;

   enum  { moveSensitivity = 100, camSensitivity = 2};

   void DisableView();
   void EnableView(int x, int y);
public:
   Camera       camera;

   // These will go to scene class eventually
   Shader shader;
   Scene *scene;

   CameraWindow(GlutMaster &glutMaster,
              int setWidth, int setHeight,
              int setInitPositionX, int setInitPositionY,
              const char *title);
   ~CameraWindow();

   void CallBackDisplayFunc(void);
   void CallBackReshapeFunc(int w, int h);   
   void CallBackIdleFunc(void);
   void CallBackKeyboardFunc(unsigned char key, int x, int y);
   void CallBackKeyboardUpFunc(unsigned char key, int x, int y);
   void CallBackPassiveMotionFunc(int x, int y);
   void CallBackMouseFunc(int button, int state, int x, int y);

   void StartSpinning(GlutMaster &glutMaster);

   void addKeyHandler(unsigned char key, KeyAction *a) 
      { keyHandlers.push_back(KeyHandler(a,key)); }

   KeyAction *removeKeyHandler(unsigned char key);
};

#endif

