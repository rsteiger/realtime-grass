#include "Shader.h"
#include "GLSL_helper.h"
#include "Bitmap.h"

using namespace std;

void Shader::attachShader(GLuint shader, const std::string &filename) {
   const GLchar *ShaderName = textFileRead(filename.c_str());

   GLint Compiled;

   //load the source
   glShaderSource(shader, 1, &ShaderName, NULL);
   printOpenGLError();

   //compile shader and print log
   glCompileShader(shader);
   printOpenGLError();

   glGetShaderiv(shader, GL_COMPILE_STATUS, &Compiled);
   printOpenGLError();

   printShaderInfoLog(shader);

   if (!Compiled) {
      cerr << "Error compiling " << filename << endl;
      return;
   }

   glAttachShader(program, shader);
   printOpenGLError();

}

void Shader::applyAttributes() {
   numLights = safe_glGetUniformLocation(program,  "uNumLights");
   lights = safe_glGetUniformLocation(program,  "uLight");
   projMatrix = safe_glGetUniformLocation(program, "uProjMatrix");
   viewMatrix = safe_glGetUniformLocation(program, "uViewMatrix");
   modelMatrix = safe_glGetUniformLocation(program, "uModelMatrix");
}

void Shader::linkProgram() {
   GLint linked;

   glLinkProgram(program);
   printOpenGLError();
   /* check shader status requires helper functions */
   glGetProgramiv(program, GL_LINK_STATUS, &linked);
   printOpenGLError();

   printProgramInfoLog(program);

   glUseProgram(program);
   printOpenGLError();

}

/*function to help load the shaders (both vertex and fragment */
GrassShader::GrassShader() {
   program = glCreateProgram();

   GLuint vs = glCreateShader(GL_VERTEX_SHADER);
   attachShader(vs,"shaders/grass.vert");

   GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
   attachShader(fs,"shaders/grass.frag");

   GLuint gs = glCreateShader(GL_GEOMETRY_SHADER_EXT);
   attachShader(gs,"shaders/grass.geom");


   glProgramParameteriEXT(program,GL_GEOMETRY_INPUT_TYPE_EXT,GL_POINTS);
   printOpenGLError();
   glProgramParameteriEXT(program,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_QUAD_STRIP);
   printOpenGLError();

   int temp = 32;
   glProgramParameteriEXT(program,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
   printOpenGLError();

   linkProgram();

   position = safe_glGetAttribLocation(program, "aPosition");
   direction = safe_glGetAttribLocation(program, "aDirection");
   time = safe_glGetUniformLocation(program,  "uTime");
   texture = safe_glGetUniformLocation(program,  "uTexture");
   alpha = safe_glGetUniformLocation(program,  "uAlpha");
   noise = safe_glGetUniformLocation(program,  "uNoise");

   /* get handles to attribute data */
   applyAttributes();

   try {
      grassTex = Bitmap("shaders/grass.bmp");
      grassAlpha = Bitmap("shaders/grass.alpha.bmp");
      grassNoise = Bitmap("shaders/grass.noise.bmp");
   } catch (char *e) {
      cerr << "Error loading image: " << e << endl;
   }

   glGenTextures(1, &grassTexLoc);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, grassTexLoc);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassTex.sizeX, grassTex.sizeY,
                0, GL_RGB, GL_UNSIGNED_BYTE, grassTex.data);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glGenTextures(1, &grassAlphaLoc);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, grassAlphaLoc);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassAlpha.sizeX, grassAlpha.sizeY,
                0, GL_RGB, GL_UNSIGNED_BYTE, grassAlpha.data);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glGenTextures(1, &grassNoiseLoc);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, grassNoiseLoc);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassNoise.sizeX, grassNoise.sizeY,
                0, GL_RGB, GL_UNSIGNED_BYTE, grassNoise.data);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


   printf("sucessfully installed shader %d\n", program);
}

PhongShader::PhongShader() {
   program = glCreateProgram();

   GLuint vs = glCreateShader(GL_VERTEX_SHADER);
   GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

   attachShader(vs,"shaders/phong.vert");
   attachShader(fs,"shaders/phong.frag");
   linkProgram();

   position = safe_glGetAttribLocation(program, "aPosition");
   color = safe_glGetUniformLocation(program,  "uColor");
   normal = safe_glGetAttribLocation(program,  "aNormal");
   applyAttributes();

   printf("successfully installed shader %d\n",program);
}
