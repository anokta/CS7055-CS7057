#ifndef TEXTURED_SIMPLE_MESH_H
#define TEXTURED_SIMPLE_MESH_H

#include "MeshBase.h"

class TexturedSimpleMesh : public MeshBase
{
public:
	TexturedSimpleMesh(const std::vector<glm::vec3> &v, const std::vector<glm::vec3> &n, const std::vector<glm::vec2> &uv);
	
	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);

	inline void SetTexture(GLuint id) { textureID = id; }

protected:
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	GLuint nSize, uvSize;

	GLuint textureID;
};

#endif