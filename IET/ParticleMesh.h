#ifndef PARTICLE_MESH_H
#define PARTICLE_MESH_H

#include "MeshBase.h"

class ParticleMesh : public MeshBase
{
public:
	ParticleMesh(const std::vector<glm::vec3> &v);

	void GenerateBuffer();

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);
};

#endif