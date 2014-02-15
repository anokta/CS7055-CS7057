#include "Ellipsoid.h"

using namespace glm;

Ellipsoid::Ellipsoid(vec3 &p, quat &o, vec3 &s, float d) : RigidBody(p, o, 2.0f * s)
{
	type = BODY_TYPE::ELLIPSOID;

	float mass = d * 4.0f / 3.0f * scale.x * scale.y * scale.z;
	massInverse = 1.0f / mass;
	
	mat3 I = mass / 5.0f * mat3(
		vec3(scale.y * scale.y + scale.z * scale.z, 0, 0),
		vec3(0, scale.x * scale.x + scale.z * scale.z, 0),
		vec3(0, 0, scale.x * scale.x + scale.y * scale.y)
		);
	inverseI = I._inverse();
}