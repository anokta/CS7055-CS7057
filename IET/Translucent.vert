#version 330

in vec3 vPosition;	
in vec3 vNormal;						

out vec3 fPosition;
out vec3 fNormal;	

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;				  
					     
const float EtaR = 0.60;
const float EtaG = 0.67; // Ratio of indices of refraction
const float EtaB = 0.79;
const float FresnelPower = 5.0;
const float F = ((1.0-EtaG) * (1.0-EtaG)) / ((1.0+EtaG) * (1.0+EtaG));
out vec3 Reflect;
out vec3 RefractR;
out vec3 RefractG;
out vec3 RefractB;
out float Ratio;

uniform vec3 fEye;

void main()
{

    fPosition = (V * M * vec4(vPosition, 1.0)).xyz;
	fNormal = (V * M * vec4(vNormal, 0.0)).xyz;
	gl_Position = P * vec4(fPosition, 1.0);  

	Ratio = F + (1.0 - F) * pow((1.0 - dot(-normalize(fPosition-fEye), normalize(fNormal))), FresnelPower);
	
	RefractR = vec3(V * vec4(refract(normalize(fPosition-fEye), normalize(fNormal), EtaR), 0.0));
	RefractG = vec3(V * vec4(refract(normalize(fPosition-fEye), normalize(fNormal), EtaG), 0.0));
	RefractB = vec3(V * vec4(refract(normalize(fPosition-fEye), normalize(fNormal), EtaB), 0.0));

	Reflect = vec3(V * vec4(reflect(normalize(fPosition-fEye), normalize(fNormal)), 0.0));


}