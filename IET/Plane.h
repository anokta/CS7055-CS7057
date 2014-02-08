#ifndef PLANE_H
#define PLANE_H

#include "RigidBody.h"

class Plane : public RigidBody
{

public:
	Plane(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec2 &s = glm::vec2(1,1), float d = 1.0f);

};

#endif