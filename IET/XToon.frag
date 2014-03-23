// Fragment Shader for X-Toon Shading

#version 330       

uniform float diffuseIntensity;
in vec3 diffuseDirection;

in vec3 fNormal;

uniform sampler2D fTexture;

in float D;

void main() 
{                        
	float intensity = max(0.0f, dot(normalize(fNormal), -diffuseDirection));

	gl_FragColor = vec4(texture(fTexture, vec2(intensity, D)).rgb, 1.0f);
}
