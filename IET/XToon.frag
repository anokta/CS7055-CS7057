// Fragment Shader for X-Toon Shading

#version 330       
in vec3 diffuseDirection;

uniform float specularShininess;

in vec3 fPosition;
in vec3 fNormal;
in vec3 fEye;

uniform float zMin, zMax;

uniform int xtoonType;

uniform sampler2D fTexture;

void main() 
{                        
	float D;
	
	vec3 viewDirection = normalize(fEye - fPosition);
	
	switch(xtoonType)
	{
	case 0: // loa
		{
			float z = length(fEye - fPosition);
			D = 1 - log(z/zMin) / log(zMax/zMin);
		}
		break;
	case 1: // silhouette-backlighting
		D = pow(dot(normalize(fNormal), normalize(viewDirection)), specularShininess);
		break;
	case 2: // perspective projection
		{
			float z = length(fEye - fPosition);
			D = 1 - log(z/zMin) / log(zMax/zMin);
		}
		break;
	case 3: // specular highlighting
		D = pow(dot(normalize(viewDirection), reflect (normalize (diffuseDirection), normalize(fNormal))), specularShininess);
		break;
	}

	float intensity = max(0.0f, dot(normalize(fNormal), -diffuseDirection));

	gl_FragColor = vec4(texture(fTexture, vec2(intensity, D)).rgb, 1.0f);
}
