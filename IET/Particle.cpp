#include "Particle.h"

#include "EntityManager.h"

#define EPSILON 0.175f
#define ELASTICITY 0.3f
#define FRICTION 0.2f

using namespace glm;

Particle::Particle(vec3 &p)
{
	position = p;
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

//bool Particle::IsColliding(ObstaclePlane * plane)
//{
//	return (dot(position - plane->GetPosition(), plane->GetNormal()) < EPSILON) && (dot(plane->GetNormal(), velocity) < 0);
//}

//void Particle::ResolveCollision(ObstaclePlane * plane)
//{
//	// reverse velocity
//	vec3 vn = dot(plane->GetNormal(), velocity) * plane->GetNormal();
//	vec3 vt = velocity - vn;
//
//	velocity = vt - ELASTICITY * vn;
//
//	// friction
//	if(abs(dot(plane->GetNormal(), velocity)) < EPSILON)
//	{
//		AddForce(-FRICTION * vt);
//	}
//
//	// spring penalty
//	AddForce(100 * abs(dot(position- plane->GetPosition(), plane->GetNormal())) * plane->GetNormal());
//}