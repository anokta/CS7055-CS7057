#ifndef XTOON_MESH_H
#define XTOON_MESH_H

#include "SimpleMesh.h"

class XToonMesh : public MeshBase
{
public:
	XToonMesh(const std::vector<glm::vec3> &v, const std::vector<GLuint> &i, const std::vector<glm::vec3> &n);
	~XToonMesh();

	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);

	inline void SetTexture(GLuint id) { textureID = id; }
	void ChangeTexture(std::string &texturePath);

private:
	std::vector<glm::vec3> normals;
	GLuint nSize;

	std::vector<GLuint> indices;
	GLuint iboID;

	GLuint textureID;
};

#endif