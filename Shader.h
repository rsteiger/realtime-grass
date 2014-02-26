#ifndef __SHADER_H__
#define __SHADER_H__

#include "glutMaster/glutMaster.h"
#include "Bitmap.h"

class Shader {
   public:
      int program;
      GLint modelMatrix;
      GLint viewMatrix;
      GLint projMatrix;
      GLint numLights;
      GLint lights;

      Shader() {};
      void attachShader(GLuint shader, const std::string &filename);
      void applyAttributes();
      void linkProgram();
};

class PhongShader : public Shader {
   public:
      GLint position;
      GLint color;
      GLint normal;

      PhongShader();
};


class GrassShader : public Shader {
   public:
      GLint position;
      GLint direction;
      GLint time;
      GLint texture;
      GLint alpha;
      GLint noise;

      GLuint grassTexLoc;
      GLuint grassAlphaLoc;
      GLuint grassNoiseLoc;

      Bitmap grassTex;
      Bitmap grassAlpha;
      Bitmap grassNoise;

      GrassShader();
};

#endif
