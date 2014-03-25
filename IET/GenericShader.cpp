#include "GenericShader.h"

#include <fstream>
#include <sstream>

using namespace std;

string GenericShader::LoadShaderFile(string path)
{
	ifstream sourceFile("..\\IET\\" + path);

	if(sourceFile)
	{
		string source((istreambuf_iterator<char>(sourceFile)), (istreambuf_iterator<char>()));

		sourceFile.close();

		return source;
	}

	return "Shader file could not be opened! \n\n\n";
}

GenericShader::GenericShader(string vShaderFilepath, string fShaderFilepath, string n)
{
	name = n;

	vShaderID = createShader(vShaderFilepath, GL_VERTEX_SHADER);
	fShaderID = createShader(fShaderFilepath, GL_FRAGMENT_SHADER);

	programID = createShaderProgram();
	
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vColorID = glGetAttribLocation(programID, "vColor");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	
	modelID = glGetUniformLocation(programID, "M");
	viewID = glGetUniformLocation(programID, "V");
	projectionID = glGetUniformLocation(programID, "P");
	
	eyeID = glGetUniformLocation(programID, "vEye");
	targetID = glGetUniformLocation(programID, "vTarget");

	ambientColorID = glGetUniformLocation(programID, "ambientColor");
	ambientIntensityID = glGetUniformLocation(programID, "ambientIntensity");

	diffuseColorID = glGetUniformLocation(programID, "diffuseColor");
	diffuseIntensityID = glGetUniformLocation(programID, "diffuseIntensity");
	diffuseDirectionID = glGetUniformLocation(programID, "vDiffuseDirection");

	specularColorID = glGetUniformLocation(programID, "specularColor");
	specularIntensityID = glGetUniformLocation(programID, "specularIntensity");
	specularShininessID = glGetUniformLocation(programID, "specularShininess");

	orenNayarRoughnessID = glGetUniformLocation(programID, "roughness");

	translucentEtaID = glGetUniformLocation(programID, "Eta");

	vUvID = glGetAttribLocation(programID, "vUv");
	fTextureID = glGetUniformLocation(programID, "fTexture");

	vTangentID = glGetAttribLocation(programID, "vTangent");
	fNormalTextureID = glGetUniformLocation(programID, "fNormalTexture");


	fCubeTextureID = glGetUniformLocation(programID, "fCubeTexture");
	
	zMaxID = glGetUniformLocation(programID, "zMax");
	zMinID = glGetUniformLocation(programID, "zMin");

	xtoonTypeID = glGetUniformLocation(programID, "xtoonType");
}

GenericShader::~GenericShader()
{
	glDeleteProgram(programID);
}

void GenericShader::UseProgram()
{
	glUseProgram(programID);
}


void GenericShader::BindTexture(GLuint textureID, TextureType type)
{
	glEnable(GL_TEXTURE_2D);

	if(type == TextureType::DIFFUSE)
	{
		glUniform1i(fTextureID, 0);
		glActiveTexture(GL_TEXTURE0);
	}
	else
	{
		glUniform1i(fNormalTextureID, 1);
		glActiveTexture(GL_TEXTURE1);
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
}

void GenericShader::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

void GenericShader::BindCubeTexture(GLuint textureID)
{
	glEnable(GL_TEXTURE_CUBE_MAP);

	glUniform1i(fCubeTextureID, 0);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void GenericShader::UnbindCubeTexture()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDisable(GL_TEXTURE_CUBE_MAP);
}

void GenericShader::SetViewMatrix(const glm::mat4 &matrix) 
{
	setMatrix(viewID, &matrix[0][0]); 
	V = matrix;
}

void GenericShader::SetProjectionMatrix(const glm::mat4 &matrix) 
{
	setMatrix(projectionID, &matrix[0][0]); 
	P = matrix;
}

void GenericShader::SetModelMatrix(const glm::mat4 &matrix) 
{
	setMatrix(modelID, &matrix[0][0]); 
	M = matrix;
}

void GenericShader::setMatrix(GLuint matrixID, const GLfloat * matrixData)
{
	glUseProgram(programID);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, matrixData);
}

void GenericShader::SetEyeVector(const glm::vec3 &eye)
{
	glUseProgram(programID);
	glUniform3f(eyeID, eye.x, eye.y, eye.z);
}

void GenericShader::SetTargetVector(const glm::vec3 &target)
{
	glUseProgram(programID);
	glUniform3f(targetID, target.x, target.y, target.z);
}

void GenericShader::SetAmbientLight(glm::vec3 color, float intensity)
{
	glUseProgram(programID);
	glUniform3f(ambientColorID, color.r, color.g, color.b);
	glUniform1f(ambientIntensityID, intensity);
}

void GenericShader::SetDirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity)
{
	glUseProgram(programID);
	glUniform3f(diffuseColorID, color.r, color.g, color.b);
	glUniform1f(diffuseIntensityID, intensity);
	glUniform3f(diffuseDirectionID, direction.x, direction.y, direction.z);
}

void GenericShader::SetSpecularComponent(glm::vec3 color, float intensity, float shininess)
{
	glUseProgram(programID);
	glUniform3f(specularColorID, color.r, color.g, color.b);
	glUniform1f(specularIntensityID, intensity);
	glUniform1f(specularShininessID, shininess);
}

void GenericShader::SetRoughness(float roughness)
{
	glUseProgram(programID);
	glUniform1f(orenNayarRoughnessID, roughness);
}

void GenericShader::SetTranslucentEta(glm::vec3 &eta)
{
	glUseProgram(programID);
	glUniform3f(translucentEtaID, eta.r, eta.g, eta.b);
}

void GenericShader::SetXtoonType(int type)
	{
	glUseProgram(programID);
	glUniform1i(xtoonTypeID, type);
}

void GenericShader::SetDistanceThresholds(float zMin, float zMax)
{
	glUseProgram(programID);
	glUniform1f(zMinID, zMin);
	glUniform1f(zMaxID, zMax);
}

GLuint GenericShader::createShader(string filepath, GLenum type)
{
	// create a shader object
	GLuint id = glCreateShader(type);

	if (id) 
	{
		// Read the source file
		string sourceString = LoadShaderFile(filepath);

		// Load the source into shader
		const GLchar * source = sourceString.c_str();
		glShaderSource(id, 1, &source, NULL);

		// Compile the shader
		glCompileShader(id);

		GLint success;
		// check for shader related errors using glGetShaderiv
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar InfoLog[1024] = { 0 }; 
			glGetShaderInfoLog(id, sizeof(InfoLog), NULL, InfoLog);
			stringstream str;
			cout << "Error compiling shader type " << type << ": '" << InfoLog <<"'";	

		}

		return id;
	}
	else
	{
		fprintf(stderr, "Error creating shader type %d with filepath %s\n", type, filepath); 

		return -1;
	}
}

GLuint GenericShader::createShaderProgram()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint id = glCreateProgram();

	if (id) 
	{
		// Attach shaders to the program
		glAttachShader(id, vShaderID);
		glAttachShader(id, fShaderID);

		// After compiling all shader objects and attaching them to the program, we can finally link it
		glLinkProgram(id);

		// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
		glValidateProgram(id);

		return id;
	}
	else
	{
		fprintf(stderr, "Error creating shader program\n");

		return -1;
	}
}
