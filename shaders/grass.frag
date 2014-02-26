//#define NORMAL

uniform sampler2D uTexture;
uniform sampler2D uAlpha;

varying vec2 oTexCoord;
varying float oDiffuse;
varying float oSpecular;
varying float age;

#ifdef NORMAL
varying float normal;
#endif

uniform mat4 uViewMatrix;

void main() {  
   #ifdef NORMAL
   if ( normal > 0.8 ) {
      gl_FragColor = vec4(1.0, 0, 0, 1.0);
      return;
   }
   #endif

   #ifndef NORMAL 
   if (texture2D(uAlpha,oTexCoord).x < .1 || oTexCoord.y > 0.99)
      discard;
   #endif
 
   vec4 baseColor = texture2D(uTexture, oTexCoord);
   baseColor += age * vec4(0.5,0.5,0.3,1.0);
   vec3 ambient = baseColor.xyz * .2;
   vec3 color = ambient;
   color += clamp(baseColor.xyz*oDiffuse+ vec3(oSpecular),0.0,1.0);
   gl_FragColor = vec4(color,1.0);
}

