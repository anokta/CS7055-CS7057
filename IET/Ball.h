#ifndef BALL_H
#define BALL_H

#include "RigidBody.h"

class Ball : public RigidBody
{

public:
	Ball(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), float radius = 0.5f, float d = 1.0f);

};

#endif