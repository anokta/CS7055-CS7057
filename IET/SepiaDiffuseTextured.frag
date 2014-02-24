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
	 vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity * max(0.0, dot(normalize(fNormal), -diffuseDirection));	

	 vec4 color =  texture(fTexture, fUv) * (ambientC + diffuseC);

	 vec4 outputColor = color;
	 outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
     outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);    
     outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);

	 gl_FragColor = outputColor;
}