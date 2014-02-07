#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm\glm.hpp"

#include "Updatable.h"

class Particle : public Updatable
{
public:
	Particle(glm::vec3 &p = glm::vec3());

	inline glm::vec3 GetPosition() const { return position; }
	inline glm::vec3 GetVelocity() const { return velocity; }
	inline glm::vec3 GetForce() const { return force; }

	inline void SetPosition(const glm::vec3 &p) { position = p; }
	inline void SetVelocity(const glm::vec3 &v) { velocity = v; }
	inline void SetForce(const glm::vec3 &f) { force = f; }

	inline void AddForce(const glm::vec3 &f) { force += f; }

	//bool IsColliding(ObstaclePlane * plane);
	//void ResolveCollision(ObstaclePlane * plane);
	
	void Update(float deltaTime);

protected:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;

	float mass;
};

#endif