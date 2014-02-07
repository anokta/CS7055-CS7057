// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;
uniform float diffuseIntensity;
in vec3 diffuseDirection;

uniform float roughness;

uniform vec3 specularColor;
uniform float specularIntensity;
uniform float specularShininess;

uniform vec3 fEye;

in vec3 fPosition;
in vec3 fNormal;

uniform sampler2D fTexture;
in vec2 fUv;

void main() 
{   
	// ambient                 	
	vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;
	   
	// diffuse
	vec3 vNorm = normalize(fEye - fPosition);
	vec3 nNorm = normalize(fNormal);

	float nDot = dot(nNorm, -normalize(diffuseDirection));
	float vDot = dot(nNorm, vNorm);
	
	float thetaI = acos(nDot);
	float thetaR = acos(vDot);
	
	float cosPhiDiff = dot(normalize(vNorm - (nNorm * vDot)), normalize(-diffuseDirection - (nNorm * nDot)));
	
	float alpha = max(thetaI, thetaR);
	float beta = min(thetaI, thetaR);

	float sigma2 = roughness * roughness;
	float A = 1.0 - 0.5 * sigma2 / (sigma2 + 0.33);
	float B = 0.45 * sigma2 / (sigma2 + 0.09);
	
	B *=  max(0.0, cosPhiDiff) * sin(alpha) * tan(beta);

	vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity * max(0.0, nDot) * (A + B);

	// specular
	if(nDot > 0.0f)
	{
		vec3 H = normalize(vNorm - diffuseDirection);
		vec4 specularC = vec4(specularColor, 1.0f) * specularIntensity * max(0.0, pow(dot(nNorm, H), specularShininess));

		diffuseC += specularC;
	}

	// final color
	gl_FragColor = texture(fTexture, fUv) * (ambientC + diffuseC);

}
