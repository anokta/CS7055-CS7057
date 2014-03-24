#ifndef TERRAIN_H
#define TERRAIN_H

#include "RigidBody.h"

class Terrain : public RigidBody
{

public:
	Terrain(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec3 &s = glm::vec3(1.0f));

};

#endif