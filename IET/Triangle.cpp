#include "Triangle.h"

using namespace glm;

Triangle::Triangle(vec3 &p, quat &o, vec2 &s, float d) : RigidBody(p, o, vec3(s[0], 1.0f, s[1]))//0.0001f, s[1]))
{
	type = BODY_TYPE::TRIANGLE;

	float mass = d * scale.x * scale.z;
	massInverse = 1.0f / mass;

	mat3 I = mass/12.0f * mat3(
		vec3(scale.y * scale.y + scale.z * scale.z, 0, 0),
		vec3(0, scale.x * scale.x + scale.z * scale.z, 0),
		vec3(0, 0, scale.x * scale.x + scale.y * scale.y)
		);
	inverseI = I._inverse();
}