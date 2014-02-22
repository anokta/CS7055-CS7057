#include "Particle.h"

#include "EntityManager.h"

#define EPSILON 0.05f
#define ELASTICITY 0.3f
#define FRICTION 0.2f

using namespace glm;

Particle::Particle(vec3 &p) : Entity(p)
{
	velocity = vec3();
	force = vec3();

	mass = 1;

	EntityManager::GetInstance()->AddUpdatable(this);
}

void Particle::Update(float deltaTime)
{	
	//velocity += (force/mass) * deltaTime;
	//position += velocity * deltaTime;
	position += velocity * deltaTime + (force/mass * deltaTime * deltaTime) / 2.0f; 
	velocity += (force/mass) * deltaTime;

	force = vec3();
}

bool Particle::IsColliding(vec3 &bodyPosition, vec3 &normal)
{
	return (dot(position - bodyPosition, normal) < EPSILON) && (dot(normal, velocity) < 0);
}

void Particle::RespondCollision(vec3 &bodyPosition, vec3 &normal)
{
	// reverse velocity
	vec3 vn = dot(normal, velocity) * normal;
	vec3 vt = velocity - vn;

	velocity = vt - ELASTICITY * vn;

	// friction
	if(abs(dot(normal, velocity)) < EPSILON)
	{
		AddForce(-FRICTION * vt);
	}

	// spring penalty
	AddForce(300 * abs(dot(position- bodyPosition, normal)) * normal);
}