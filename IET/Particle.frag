// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

in vec4 fColor;

void main() 
{                        
	 vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;
	 
	 gl_FragColor =  fColor * ambientC;
}
