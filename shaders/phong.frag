#define MAX_LIGHTS 8

varying vec3 vColor;
varying vec4 vNormal;
varying vec4 vPosition;
varying vec3 vLightDir[MAX_LIGHTS];

uniform int uNumLights;

void main() {
   vec3 N, L, E, R, diffuse, color, specular;
   color = vec3(0,0,0);

   N = normalize(vNormal.xyz);
   for (int i = 0; i < uNumLights; i++) {
      L = normalize(vLightDir[i]);
      float lambert = dot(N,L);
      if (lambert > 0.0) {
         diffuse = clamp(vColor * lambert,0.0,1.0);
         E = normalize(-vPosition).xyz;
         R = reflect(-L,N);
         specular = clamp(vec3(1,1,1)*pow(max(dot(R,E), 0.0), 2.0),0.0,1.0);
         color += clamp(diffuse+specular,0.0,1.0);
      }
   }
   
   gl_FragColor = vec4(color.r,
                       color.g,
                       color.b, 1.0);
}
