#include "Tetrahedron.h"

using namespace glm;

Tetrahedron::Tetrahedron(vec3 &p, quat &o, vec3 &s, float d) : RigidBody(p, o, s, d)
{
	type = BODY_TYPE::TETRAHEDRON;

	mat3 I = mat3(
		vec3(3.0f*mass/5.0f*scale.y *scale.y + 3.0f*mass/20.0f*scale.x*scale.z , 0, 0),
		vec3(0, 3.0f*mass/10.0f*scale.x*scale.z, 0),
		vec3(0, 0, 3.0f*mass/5.0f*scale.y *scale.y + 3.0f*mass/20.0f*scale.x*scale.z )
		);
	inverseI = I._inverse();

	mass = d * scale.x * scale.y * scale.z;
}