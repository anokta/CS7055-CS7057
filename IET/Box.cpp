#include "Box.h"

using namespace glm;

Box::Box(vec3 &p, quat &o, vec3 &s, float d) : RigidBody(p, o, s)
{
	type = BODY_TYPE::BOX;
	
	float mass = d * scale.x * scale.y * scale.z;
	massInverse = 1.0f / mass;

	mat3 I = mass/12.0f * mat3(
		vec3(scale.y * scale.y + scale.z * scale.z, 0, 0),
		vec3(0, scale.x * scale.x + scale.z * scale.z, 0),
		vec3(0, 0, scale.x * scale.x + scale.y * scale.y)
		);
	inverseI = I._inverse();
}