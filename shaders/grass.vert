#define MAPSCALE 50.0
#define HEIGHTSCALE 3.0

attribute vec4 aPosition;
attribute float aDirection;

varying vec4 vPosition;
varying float distance;
varying float direction;

uniform mat4 uProjMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform sampler2D uNoise;

float getHeight(in float x, in float z) {
   return HEIGHTSCALE * length(texture2D(uNoise, vec2(mod(x/MAPSCALE, 1.0),mod(z/MAPSCALE, 1.0))).rgb);
}

void main() {
   vec4 oPosition;
   mat4 modelView = uViewMatrix * uModelMatrix;

   vec4 cPosition = uViewMatrix * vec4(0,0,-1,1);
   vPosition = uModelMatrix * aPosition;
   vPosition.y += getHeight(vPosition.x, vPosition.z) - 1.0;

   oPosition = uViewMatrix * vPosition;
   gl_Position = uProjMatrix * oPosition; 
   distance = 2.0 / (0.1 + dot(oPosition.xyz, oPosition.xyz));
   direction = aDirection;
}
