#define MAX_LIGHTS 8

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

attribute vec3 aPosition;

void main() {
   mat4 modelView = uViewMatrix * uModelMatrix;
   vec4 vPosition = modelView * vec4(aPosition.x, aPosition.y, aPosition.z, 1);
   gl_Position = uProjMatrix*vPosition;
}
