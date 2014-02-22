#ifndef PARTICLE_H
#define PARTICLE_H

#include "Entity.h"
#include "RigidBody.h"

class Particle : public Entity
{
public:
	Particle(glm::vec3 &p = glm::vec3());

	inline glm::vec3 GetVelocity() const { return velocity; }
	inline glm::vec3 GetForce() const { return force; }

	inline void SetVelocity(const glm::vec3 &v) { velocity = v; }
	inline void SetForce(const glm::vec3 &f) { force = f; }

	inline void AddForce(const glm::vec3 &f) { force += f; }

	bool IsColliding(glm::vec3 &p, glm::vec3 &n);
	void RespondCollision(glm::vec3 &p, glm::vec3 &n);
	
	void Update(float deltaTime);

protected:
	glm::vec3 velocity;
	glm::vec3 force;

	float mass;
};

#endif