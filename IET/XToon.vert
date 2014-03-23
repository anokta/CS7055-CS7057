// Vertex Shader for X-Toon Shading

#version 330                                                                  
                                                              
in vec3 vPosition;	
in vec3 vNormal;										

out vec3 fPosition;
out vec3 fNormal;

uniform vec3 vDiffuseDirection;	
out vec3 diffuseDirection;		

uniform vec3 vEye;
out vec3 fEye;

//uniform float zMin, zMax;
out float D;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;				  
					                                                           
void main()                                                                     
{                                                                                  
    fPosition = (V * M * vec4(vPosition, 1.0)).xyz;
	fNormal = (V * M * vec4(vNormal, 0.0)).xyz;
	gl_Position = P * vec4(fPosition, 1.0);  
	
	diffuseDirection = (V * vec4(vDiffuseDirection, 0.0)).xyz;
	fEye = vec3(V * vec4(vEye, 1.0));

	float zMax = 80.0f;
	float zMin = 25.0f;

	//depth of field
	float z = length(fEye - fPosition);
	D = 1 - log(z/zMin) / log(zMax/zMin);

	//backlight
	//vec3 viewDirection = normalize(fEye - fPosition);
	//D = pow(dot(normalize(fNormal), normalize(viewDirection)), 1);

	//D = pow(dot(normalize(viewDirection), reflect (normalize (diffuseDirection), normalize(fNormal))), 8);
}