#ifndef SKYBOX_MESH_H
#define SKYBOX_MESH_H

#include "MeshBase.h"

class SkyboxMesh : MeshBase
{
public:
	SkyboxMesh(const std::vector<glm::vec3> &v);

	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);
	
	inline void SetTexture(GLuint id) { textureID = id; }

private:
	GLuint textureID;
};

#endif