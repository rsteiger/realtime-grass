#define MAX_LIGHTS 8

uniform vec3 uColor;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform int uNumLights;
uniform vec3 uLight[MAX_LIGHTS];

attribute vec3 aPosition;
attribute vec3 aNormal;

varying vec4 vPosition;
varying vec4 vNormal;
varying vec3 vColor;
varying vec3 vLightDir[MAX_LIGHTS];

void main()
{  
   mat4 modelView = uViewMatrix * uModelMatrix;
   vPosition = modelView * vec4(aPosition.x, aPosition.y, aPosition.z, 1);
   gl_Position = uProjMatrix * vPosition;

   vNormal = uModelMatrix * vec4(aNormal.x, aNormal.y, aNormal.z, 0);
   vColor = uColor;

   for (int i = 0; i < uNumLights; i++) {
      vLightDir[i] = (uViewMatrix * vec4(uLight[i],1) - vPosition).xyz;
   }
}
