#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "RigidBody.h"

class Ellipsoid : public RigidBody
{

public:
	Ellipsoid(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec3 &s = glm::vec3(0.5f), float d = 1.0f);

};

#endif