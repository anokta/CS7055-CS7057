// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;
uniform float diffuseIntensity;
in vec3 diffuseDirection;

uniform vec3 fEye;
uniform vec3 specularColor;
uniform float specularIntensity;
uniform float specularShininess;

in vec3 fPosition;
in vec3 fNormal;

uniform sampler2D fTexture;
in vec2 fUv;

void main() 
{               
	float nDot = max(0.0, dot(normalize(fNormal), -diffuseDirection));

	vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;
	vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity * nDot;	

	if(nDot > 0.0f)
	{
		vec3 H = normalize(normalize(fEye - fPosition) - diffuseDirection);
		vec4 specularC = vec4(specularColor, 1.0f) * specularIntensity * max(0.0, pow(dot(normalize(fNormal), H), specularShininess));

		diffuseC += specularC;
	}

	gl_FragColor =  texture(fTexture, fUv) * (ambientC + diffuseC);


}
