#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Particle.h"

#include "Drawable.h"
#include "MeshBase.h"

#include "RigidBodyModel.h"

class ParticleSystem : public Drawable
{
public:
	ParticleSystem(GenericShader * s, int size);
	~ParticleSystem();

	void Draw();

	void ApplyGravity();
	void ApplyViscousDrag();
	
	void ApplyGlobalForce(glm::vec3 &f);
	void ApplyEQForces(float * bands);

	void HandleCollisions(std::vector<RigidBodyModel*> & obstacles);

private:
	std::vector<Particle*> particles;

	GenericShader * shader;
	MeshBase * model;

};

#endif