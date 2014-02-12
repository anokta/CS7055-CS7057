#ifndef BUMPED_TEXTURED_MESH_H
#define BUMPED_TEXTURED_MESH_H

#include "TexturedIndexedMesh.h"

class BumpedTexturedMesh : public TexturedIndexedMesh
{
public:
	BumpedTexturedMesh(const std::vector<glm::vec3> &v, const std::vector<GLuint> &i, const std::vector<glm::vec3> &n, const std::vector<glm::vec2> &uv, const std::vector<glm::vec4> &t);
	
	void GenerateBuffer();

	void Render(GenericShader * shader);
	void SetShader(GenericShader * shader);

	inline void SetNormalTexture(GLuint id) { normalTextureID = id; }

private:
	std::vector<glm::vec4> tangents;

	GLuint tangentVboID;

	GLuint normalTextureID;
};

#endif