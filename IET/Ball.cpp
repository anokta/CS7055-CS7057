#include "Ball.h"

using namespace glm;

Ball::Ball(vec3 &p, quat &o, float radius, float d) : RigidBody(p, o, vec3(2.0f * radius), d)
{
	type = BODY_TYPE::BALL;

	mat3 I = (2.0f * mass / 5.0f) * (radius * radius) * mat3(
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1)
		);
	inverseI = I._inverse();

	mass = d * 4.0f / 3.0f * radius * radius * radius;
}