// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;
uniform float diffuseIntensity;
in vec3 diffuseDirection;

in vec3 fNormal;

uniform sampler2D fTexture;
in vec2 fUv;

void main() 
{                        
	 vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;
	 vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity;	

	 float intensity = dot(normalize(fNormal), -diffuseDirection);
	 vec4 outputColor =  texture(fTexture, fUv);

	 if (intensity > 0.95)
		outputColor *= 1.0f;
	else if (intensity > 0.5)
		outputColor *= 0.65f;
	else if (intensity > 0.25)
		outputColor *= 0.4f;
	else
		outputColor *= 0.2f;

	gl_FragColor = outputColor;
}
