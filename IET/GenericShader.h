#ifndef GENERIC_SHADER_H
#define GENERIC_SHADER_H

#include <iostream>

#include <GL/glew.h>

#include "glm/glm.hpp"

class GenericShader 
{

public:
	static std::string LoadShaderFile(std::string path);

	GenericShader() {}
	GenericShader(std::string vShaderFilepath, std::string fShaderFilepath, std::string n = "Default");
	~GenericShader();
	
	// Getters
	inline const std::string GetName() { return name; }

	inline const GLuint GetVertexPositionID() { return vPositionID; }
	inline const GLuint GetVertexColorID() { return vColorID; }
	inline const GLuint GetVertexNormalID() { return vNormalID; }
	inline const GLuint GetVertexUVID() { return vUvID; }
	inline const GLuint GetVertexTangentID() { return vTangentID; }
	
	inline const glm::mat4& GetViewMatrix() { return V; }
	inline const glm::mat4& GetProjectionMatrix() { return P; }
	inline const glm::mat4& GetModelMatrix() { return M; }
	
	inline GLuint GetProgramID() { return programID; }

	// Setters
	void SetViewMatrix(const glm::mat4 &matrix);
	void SetProjectionMatrix(const glm::mat4 &matrix);
	void SetModelMatrix(const glm::mat4 &matrix);

	void SetEyeVector(const glm::vec3 &eye);

	void SetAmbientLight(glm::vec3 color, float intensity);
	void SetDirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity);
	void SetSpecularComponent(glm::vec3 color, float intensity, float shininess);
	void SetRoughness(float roughness);
	
	enum TextureType { DIFFUSE, NORMAL };
	void BindTexture(GLuint textureID, TextureType type = TextureType::DIFFUSE);
	void UnbindTexture();

	void BindCubeTexture(GLuint textureID);
	void UnbindCubeTexture();

	void UseProgram();

protected:
	std::string name;

	GLuint createShader(std::string filepath, GLenum type);
	GLuint createShaderProgram();

	GLuint vShaderID, fShaderID;
	GLuint programID;
	
	GLuint vPositionID, vColorID, vNormalID;
	GLuint vUvID, fTextureID;
	GLuint vTangentID;
	GLuint viewID, projectionID, modelID;
	GLuint eyeID;
	
	GLuint fNormalTextureID;
	GLuint fCubeTextureID;

	GLuint ambientColorID, ambientIntensityID;	
	GLuint diffuseColorID, diffuseIntensityID, diffuseDirectionID;
	GLuint specularColorID, specularIntensityID, specularShininessID;
	GLuint orenNayarRoughnessID;
	
	glm::mat4 V, P, M;
	
	void setMatrix(GLuint matrixID, const GLfloat * matrixData);
};

#endif