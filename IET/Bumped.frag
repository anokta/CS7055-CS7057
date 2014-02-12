// Fragment Shader for imported 3D models

#version 330       

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;
uniform float diffuseIntensity;
in vec3 diffuseDirection;

uniform vec3 fEye;
uniform vec3 specularColor;
uniform float specularIntensity;
uniform float specularShininess;

uniform sampler2D fTexture;

in vec3 viewTangent, lightTangent;
uniform sampler2D fNormalTexture;

in vec3 fPosition;
in vec2 fUv;

void main() 
{                        
	 vec4 ambientC = vec4(ambientColor, 1.0f) * ambientIntensity;

	 vec3 normalTangent = texture (fNormalTexture, fUv).rgb;
	 normalTangent = normalize (normalTangent * 2.0 - 1.0);

	 vec4 diffuseC = vec4(diffuseColor, 1.0f) * diffuseIntensity * max(0.0, dot(normalTangent, -normalize(lightTangent)));

	 float H = max(0.0, dot(reflect (normalize (lightTangent), normalTangent), normalize (viewTangent)));
	 vec4 specularC = vec4(specularColor, 1.0f) * specularIntensity * max(0.0, pow(H, specularShininess));

	 gl_FragColor =  texture(fTexture, fUv) * (ambientC + diffuseC + specularC);
}
