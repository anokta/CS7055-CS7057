// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;
uniform float diffuseIntensity;
in vec3 diffuseDirection;

in vec4 fColor;
in vec3 fNormal;

void main() 
{                        
	 vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;
	 vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity * max(0.0, dot(normalize(fNormal), -diffuseDirection));	

	 gl_FragColor =  fColor * (ambientC + diffuseC);
}
