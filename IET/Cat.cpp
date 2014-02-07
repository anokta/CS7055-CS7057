#include "Cat.h"

using namespace glm;

Cat::Cat(vec3 &p, quat &o, vec3 &s, float d) : RigidBody(p, o, s, d)
{
	type = BODY_TYPE::CAT;

	mat3 I = mass/12.0f * mat3(
		vec3(scale.y * scale.y + scale.z * scale.z, 0, 0),
		vec3(0, scale.x * scale.x + scale.z * scale.z, 0),
		vec3(0, 0, scale.x * scale.x + scale.y * scale.y)
		);
	inverseI = I._inverse();

	mass = d * scale.x * scale.y * scale.z;
}