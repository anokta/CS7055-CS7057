// Fragment Shader for imported 3D models

#version 330     

uniform sampler2D fTexture;
in vec2 fUv;

void main() 
{                     
	 gl_FragColor =  texture(fTexture, fUv);
}
