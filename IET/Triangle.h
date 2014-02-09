#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "RigidBody.h"

class Triangle : public RigidBody
{

public:
	Triangle(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec2 &s = glm::vec2(1, 1), float d = 1.0f);

};

#endif