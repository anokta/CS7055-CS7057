#ifndef IMAGE_2D_H
#define IMAGE_2D_H

#include "MeshBase.h"

class Image2D : public MeshBase
{
public:
	Image2D(std::string &path, glm::vec2 &p = glm::vec2(), float s = 1.0f);
	Image2D(glm::vec2 &p = glm::vec2(), float s = 1.0f, GLuint texture = 0);
	~Image2D();

	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);
	
	void ChangeTexture(std::string &texturePath);
	
	inline void SetTexture(GLuint id) { textureID = id; }

protected:
	glm::vec2 position;
	float size;

	std::vector<glm::vec2> uvs;
	GLuint uvSize;

	GLuint textureID;
	
	void generateMesh();
};

#endif