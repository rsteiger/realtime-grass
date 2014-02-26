#version 120 
#extension GL_EXT_geometry_shader4 : enable

#define WINDC 0.5
#define GWIDTH 1.0 / 20.0
#define GVARY 0.1
#define GSPRINGT 2.0
#define WINDS 2.0
#define NORMLEN 0.1

#define MAX_LIGHTS 8
//#define NORMALS

varying in vec4 vPosition[1];
varying in float distance[1];
varying in float direction[1];

varying out vec2 oTexCoord;
varying out float oDiffuse;
varying out float oSpecular;
varying out float age;
#ifdef NORMALS
varying out float normal;
#endif

uniform mat4 uProjMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;


uniform float uTime;
uniform vec3 uLight[MAX_LIGHTS];
uniform int uNumLights;

void main(void) {
   int i, j;
   int detail;
   float heightC;
   vec4 cPosition, cNormal;
   vec4 oPosition, oNormal;

   float t_age = sin(direction[0]);
   float heightmod = 0.75 + sin(5.090293 * direction[0] + 10) / 4.0; // Just a random magic number!

   mat4 modelView = uViewMatrix * uModelMatrix;

   vec4 gPosition = vPosition[0] + uModelMatrix * vec4(0,0,0,1);
   
   vec2 wind = vec2(0.4*sin(uTime + gPosition.x / 10), 0.4*sin(uTime*0.256 + gPosition.z / 10));

   vec3 disp = vec3(cos(direction[0]), 0, sin(direction[0]));

   float vary = 1.0 + GVARY * sin(GSPRINGT * uTime + direction[0]);
   wind = wind * vary;

   if ( distance[0] >= 3)
      detail = 5;
   if ( distance[0] >= 2)
      detail = 4;
   else if ( distance[0] >= 1.5)
      detail = 3;
   else if ( distance[0] >= 1)
      detail = 2;
   else 
      detail = 1;

   detail = detail * 2;
   for(j=0; j < detail+2; j++) {
      for(i=0; i < gl_VerticesIn; i++){
         heightC = j - mod(j,2);
         heightC = heightC / detail;
         age = t_age;

         // Setup Texture
         oTexCoord.x = mod(j,2);
         oTexCoord.y = heightC;

         // Setup Blade
         cPosition = vPosition[i];
         cPosition.y = cPosition.y + heightC * heightmod;
         cPosition.xz = cPosition.xz + disp.xz * mod(j, 2) * GWIDTH;
   
         // Wind
         vec3 windTrans;
         windTrans.xz = wind * pow(heightC,WINDS);
         windTrans.y = -length(wind) * pow(heightC,WINDS) * 0.5;
         cPosition.xyz = cPosition.xyz + windTrans;

         // Blade Normal
         cNormal = vec4(0.0,0.0,0.0,0.0);
         vec3 windNorm;
         windNorm.xz = wind * pow(heightC,WINDS-1) * WINDS;
         windNorm.y = heightmod - length(wind) * pow(heightC,WINDS-1) * WINDS * 0.5; 
         cNormal.xyz = cross(vec3(wind.y,0.0,wind.x),windNorm);
         if ( oNormal.y < 0 )
            oNormal = -oNormal;
         
         // Transform
         oPosition = uViewMatrix * cPosition;
         oNormal = modelView * cNormal;

         #ifdef NORMALS
         normal = 0;
         #endif NORMALS

         // Offload diffuse and specular from frag shader
         vec3 N, L, E, R;

         N = normalize(oNormal.xyz);
         L = normalize((uViewMatrix * vec4(0.0,10.0,0.0,1.0)).xyz);

         float lambert = abs(dot(N,L));
         E = normalize(-oPosition.xyz);
         R = reflect(-L,N);
         oDiffuse = lambert;
         oSpecular = .3 * clamp(pow(max(dot(R,E), 0.0), 1.3),0.0,1.0);

         // Project
         gl_Position = uProjMatrix * oPosition;
         EmitVertex();

         #ifdef NORMALS
         normal = 1;
         oPosition = uViewMatrix * (cPosition);
         gl_Position = uProjMatrix * oPosition;
         EmitVertex();

         normal = 1;
         oPosition = uViewMatrix * (cPosition + vec4(normalize(cNormal.xyz)*NORMLEN,0.0));
         gl_Position = uProjMatrix * oPosition;
         EmitVertex();

         normal = 1;
         oPosition = uViewMatrix * (cPosition);
         gl_Position = uProjMatrix * oPosition;
         EmitVertex();
         #endif
      }
   }
   EndPrimitive();   
}

