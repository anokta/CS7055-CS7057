// Fragment Shader for skybox
#version 330       
                                                                 
in vec3 fTexCoords;    
           
uniform samplerCube fCubeTexture;   

void main() 
{                
	 gl_FragColor =  texture(fCubeTexture, fTexCoords);
}
