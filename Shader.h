#ifndef __SHADER_H__
#define __SHADER_H__

#include "glutMaster/glutMaster.h"

class Shader {
   public:
      int program;
      GLint modelMatrix;
      GLint viewMatrix;
      GLint projMatrix;
      GLint position;
      GLint color;
      GLint normal;
      GLint numLights;
      GLint lights;

      Shader() {};
      Shader(const GLchar *vShaderName, const GLchar *gShaderName, const GLchar *fShaderName);
};

#endif
