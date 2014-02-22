// Vertex Shader for imported 3D models

#version 330                                                                  
                                                              
in vec3 vPosition;	
in vec4 vColor;											

out vec4 fColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;		
                                                                               
void main()                                                                     
{                                                                                  
    gl_Position = P * V * M * vec4(vPosition, 1.0);  
	
	fColor = vColor * vec4(gl_Position.y, 1.0f - gl_Position.y, 0.0f, 0.9f);
}