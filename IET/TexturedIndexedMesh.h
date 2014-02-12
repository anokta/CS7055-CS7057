#ifndef TEXTURED_INDEXED_MESH_H
#define TEXTURED_INDEXED_MESH_H

#include "TexturedSimpleMesh.h"

class TexturedIndexedMesh : public TexturedSimpleMesh
{
public:
	TexturedIndexedMesh(const std::vector<glm::vec3> &v, const std::vector<GLuint> &i, const std::vector<glm::vec3> &n, const std::vector<glm::vec2> &uv);
	~TexturedIndexedMesh();

	void GenerateBuffer();

	void Render(GenericShader * shader);

protected:
	GLuint iboID;
	
	std::vector<GLuint> indices;
};

#endif