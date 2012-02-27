#include "Shader.h"
#include "GLSL_helper.h"

/*function to help load the shaders (both vertex and fragment */
Shader::Shader(const GLchar *vShaderName, const GLchar *gShaderName, const GLchar *fShaderName) {
   GLuint VS; //handles to vert shader object
   GLuint FS; //handles to frag shader object
   GLuint GS; //handles to geom shader object
   GLint vCompiled, gCompiled, fCompiled, linked; //status of shader

   VS = glCreateShader(GL_VERTEX_SHADER);
   FS = glCreateShader(GL_FRAGMENT_SHADER);
   GS = glCreateShader(GL_GEOMETRY_SHADER_EXT);

   //load the source
   glShaderSource(VS, 1, &vShaderName, NULL);
   glShaderSource(FS, 1, &fShaderName, NULL);
   glShaderSource(GS, 1, &gShaderName, NULL);

   //compile shader and print log
   glCompileShader(VS);
   printOpenGLError();
   glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
   printShaderInfoLog(VS);

   //compile shader and print log
   glCompileShader(FS);
   printOpenGLError();
   glGetShaderiv(FS, GL_COMPILE_STATUS, &fCompiled);
   printShaderInfoLog(FS);

   //compile shader and print log
   glCompileShader(GS);
   printOpenGLError();
   glGetShaderiv(GS, GL_COMPILE_STATUS, &gCompiled);
   printShaderInfoLog(GS);

   if (!vCompiled || !fCompiled || !gCompiled) {
      printf("Error compiling shader.");
      return;
   }

   //create a program object and attach the compiled shader
   program = glCreateProgram();
   glAttachShader(program, VS);
   glAttachShader(program, FS);
   glAttachShader(program, GS);

   glProgramParameteriEXT(program,GL_GEOMETRY_INPUT_TYPE_EXT,GL_POINTS);
   glProgramParameteriEXT(program,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_POINTS);

   int temp;
   glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
   glProgramParameteriEXT(program,GL_GEOMETRY_VERTICES_OUT_EXT,temp);


   glLinkProgram(program);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetProgramiv(program, GL_LINK_STATUS, &linked);
   printProgramInfoLog(program);

   glUseProgram(program);

   /* get handles to attribute data */
   position = safe_glGetAttribLocation(program, "aPosition");
   color = safe_glGetUniformLocation(program,  "uColor");
   numLights = safe_glGetUniformLocation(program,  "uNumLights");
   lights = safe_glGetUniformLocation(program,  "uLight");
   normal = safe_glGetAttribLocation(program,  "aNormal");
   projMatrix = safe_glGetUniformLocation(program, "uProjMatrix");
   viewMatrix = safe_glGetUniformLocation(program, "uViewMatrix");
   modelMatrix = safe_glGetUniformLocation(program, "uModelMatrix");

   printf("sucessfully installed shader %d\n", program);
}
