#ifndef INDEXED_MESH_H
#define INDEXED_MESH_H

#include "SimpleMesh.h"

class IndexedMesh : public SimpleMesh
{
public:
	IndexedMesh(const std::vector<glm::vec3> &v, const std::vector<GLuint> &i, const std::vector<glm::vec4> &c, const std::vector<glm::vec3> &n);
	~IndexedMesh();

	void GenerateBuffer();

	void Render(GenericShader * shader);

private:
	GLuint iboID;

	std::vector<GLuint> indices;
};

#endif