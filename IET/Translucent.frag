#version 330

uniform samplerCube fCubeTexture;
                                          
in vec3 fTexCoords;    

in vec3 Reflect;
in vec3 RefractR;
in vec3 RefractG;
in vec3 RefractB;
in float Ratio;

void main()
{
vec3 refractColor;
refractColor.r = vec3(texture(fCubeTexture, RefractR)).r;
refractColor.g = vec3(texture(fCubeTexture, RefractG)).g;
refractColor.b = vec3(texture(fCubeTexture, RefractB)).b;

vec3 reflectColor = vec3(texture(fCubeTexture, Reflect));

vec3 color = mix(refractColor, reflectColor, Ratio);
 
 gl_FragColor = vec4(color, 1.0);

 }