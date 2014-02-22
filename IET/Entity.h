#ifndef ENTITY_H
#define ENTITY_H

#include "glm\glm.hpp"

#include "Updatable.h"

class Entity : public Updatable
{
public:
	Entity(glm::vec3 &p = glm::vec3()) : position(p) {};

	inline glm::vec3 GetPosition() const { return position; }
	
	inline void SetPosition(const glm::vec3 &p) { position = p; }

	virtual void Update(float deltaTime) = 0;

protected:
	glm::vec3 position;
};

#endif