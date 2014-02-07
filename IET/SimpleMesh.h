#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include "MeshBase.h"

class SimpleMesh : public MeshBase
{
public:
	SimpleMesh(const std::vector<glm::vec3> &v, const std::vector<glm::vec4> &c, const std::vector<glm::vec3> &n);

	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);

protected:
	std::vector<glm::vec4> colors;
	std::vector<glm::vec3> normals;

	GLuint cSize, nSize;
};

#endif