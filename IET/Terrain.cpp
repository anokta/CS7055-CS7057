#include "Terrain.h"

using namespace glm;

Terrain::Terrain(vec3 &p, quat &o, vec3 &s) : RigidBody(p, o, s)
{
	type = BODY_TYPE::TERRAIN;
	
	massInverse = 0.0f;
	inverseI = mat3(0.0f);
}