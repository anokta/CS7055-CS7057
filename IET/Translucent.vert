#version 330

in vec3 vPosition;	
in vec3 vNormal;						

out vec3 fPosition;
out vec3 fNormal;	

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;				  
					
uniform vec3 Eta;     

uniform vec3 vEye;
out vec3 fEye;

const float FresnelPower = 5.0;

out vec3 Reflect;
out vec3 RefractR;
out vec3 RefractG;
out vec3 RefractB;
out float Ratio;


void main()
{
	float etaAvg = Eta.g; //(Eta.r + Eta.g + Eta.b) / 3.0;
	float F = ((1.0-etaAvg) * (1.0-etaAvg)) / ((1.0+etaAvg) * (1.0+etaAvg));

    fPosition = (V * M * vec4(vPosition, 1.0)).xyz;
	fNormal = (V * M * vec4(vNormal, 0.0)).xyz;
	gl_Position = P * vec4(fPosition, 1.0);  

	fEye = vec3(V * vec4(vEye, 1.0));
	vec3 viewDirection = normalize(fPosition - fEye);

	vec3 n = normalize(fNormal);

	Ratio = F + (1.0 - F) * pow((1.0 - dot(-viewDirection, n)), FresnelPower);
	
	RefractR = vec3(V * vec4(refract(viewDirection, n, Eta.r), 0.0));
	RefractG = vec3(V * vec4(refract(viewDirection, n, Eta.g), 0.0));
	RefractB = vec3(V * vec4(refract(viewDirection, n, Eta.b), 0.0));

	Reflect = vec3(V * vec4(reflect(viewDirection, n), 0.0));


}