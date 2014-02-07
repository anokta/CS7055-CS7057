// Vertex Shader for imported 3D models

#version 330                                                                  
                                                              
in vec3 vPosition;	
in vec4 vColor;	
in vec3 vNormal;										

out vec3 fPosition;
out vec4 fColor;
out vec3 fNormal;

uniform vec3 vDiffuseDirection;	
out vec3 diffuseDirection;		

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;				  
					                                                           
void main()                                                                     
{                                                                                  
    fPosition = (V * M * vec4(vPosition, 1.0)).xyz;
	fNormal = (V * M * vec4(vNormal, 0.0)).xyz;
	gl_Position = P * vec4(fPosition, 1.0);  
	
	diffuseDirection = (V * vec4(vDiffuseDirection, 0.0)).xyz;

	fColor = vColor;
}