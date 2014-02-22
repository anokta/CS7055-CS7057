// Vertex Shader for imported 3D models

#version 330                                                                  
                                                              
in vec3 vPosition;							

out vec4 fColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;		
                                                                               
void main()                                                                     
{   
	vec4 fPosition = M * vec4(vPosition, 1.0);                                                                         
    gl_Position = P * V * fPosition;  
	

	fColor = vec4(fPosition.y, 0.9f * (4.0f - fPosition.y) + 0.1f * fPosition.y, 0.6f * (2.0f - fPosition.y) + 0.4f * fPosition.y, 1.0f);
}