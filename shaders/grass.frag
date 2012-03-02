//#define NORMAL

uniform sampler2D uTexture;
uniform sampler2D uAlpha;

varying vec4 oNormal;
varying vec2 oTexCoord;
varying vec4 oPosition;
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
   if (texture2D(uAlpha,oTexCoord).x < .1)
      discard;
   #endif
 
   vec4 baseColor = texture2D(uTexture, oTexCoord);
   baseColor += age * vec4(0.5,0.5,0.3,1.0);
   vec3 N, L, E, R, ambient, diffuse, specular, color;

   ambient = baseColor.xyz * .2;

   color = ambient;
   N = normalize(oNormal.xyz);
   L = normalize((uViewMatrix * vec4(0.0,10.0,0.0,1.0)).xyz);

   float lambert = abs(dot(N,L));
   if (lambert > 0.0) {
         E = normalize(-oPosition.xyz);
         R = reflect(-L,N);
         diffuse = clamp(baseColor.xyz*lambert,0.0,1.0);
         specular = vec3(.3,.3,.3) * clamp(pow(max(dot(R,E), 0.0), 1.3),0.0,1.0);
         color += clamp(diffuse+specular,0.0,1.0);
   }
   gl_FragColor = vec4(color,1.0);
}

