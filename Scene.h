#ifndef __SCENE_H__
#define __SCENE_H__

#include "glutMaster/glutMaster.h"
#include "glm/glm.hpp"
#include <vector>

class Instance {
   public:
      glm::mat4 transform;
      Instance() { };
      Instance(glm::mat4 mat) : transform(mat) { };
};

class Mesh {
   public:  
      GLuint buffObj;
      GLuint indxBuffObj;
      GLuint normalBuffObj;
      int iboLen; 
   
      Mesh() { };

      explicit Mesh(const std::string &filename);
};

class Model {
   public:
      std::vector<Mesh> meshes;
      std::vector<Instance> instances;
};

class Light {
   public:
      float x,y,z;
      Light() : x(0),y(0),z(0) { };
      Light(float mx, float my, float mz): x(mx), y(my), z(mz) { };
      Light transform(const glm::mat4 &trans) { 
         glm::vec4 vec(x,y,z,0);
         glm::vec4 vn = trans * vec;
         return Light(vn.x,vn.y,vn.z);
      }
};

class Scene {
   public:
      std::vector<Model> models;
      std::vector<Light> lights;
};

class Camera {
   public:
      enum { sensitivity = 2, moveSensitivity = 1 };
      glm::vec3 trans;
      float rot;
      float pitch;
      Camera(): rot(0), pitch(0),trans(0.0) {};
};
#endif
