// Vertex Shader for imported 3D models

#version 330                                                                  
                                                              
in vec3 vPosition;	
in vec3 vNormal;	
in vec2 vUv;	
in vec4 vTangent;								

out vec3 fPosition;
out vec3 fNormal;	
out vec2 fUv;	

uniform vec3 vDiffuseDirection;	
out vec3 diffuseDirection;	

out vec3 viewTangent, lightTangent;
uniform vec3 vEye;
out vec3 fEye;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;		
					                                                           
void main()                                                                     
{                                                                              
    fPosition = (V * M * vec4(vPosition, 1.0)).xyz;
	fNormal = (V * M * vec4(vNormal, 0.0)).xyz;
	gl_Position = P * vec4(fPosition, 1.0);  
	
	diffuseDirection = (V * vec4(vDiffuseDirection, 0.0)).xyz;
	vec3 fTangent = (V * M * vec4(vTangent.xyz, 0.0)).xyz;
	
	fUv = vUv;

	vec3 bitangent = cross (fNormal, fTangent) * vTangent.w;

	fEye = vec3(V * vec4(vEye, 1.0));
	vec3 viewDirection = normalize(fEye - fPosition);

	viewTangent = vec3 (
    dot (fTangent, viewDirection),
    dot (bitangent, viewDirection),
    dot (fNormal, viewDirection)
  );

	diffuseDirection = vec3 (
    dot (fTangent, diffuseDirection),
    dot (bitangent, diffuseDirection),
    dot (fNormal, diffuseDirection)
  );
}