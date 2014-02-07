#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"

#include "GenericShader.h"
#include "Updatable.h"

class Camera : public Updatable
{
public:
	Camera(std::vector<GenericShader*> &s, glm::vec3 &e = glm::vec3(), glm::vec3 &t = glm::vec3(), glm::vec3 &u = glm::vec3(0,1,0));

	inline glm::vec3 GetEyeVector() const { return eye; }
	inline glm::vec3 GetTargetVector() const { return target; }
	inline glm::vec3 GetUpVector() const { return up; }

	inline void SetEyeVector(const glm::vec3 &e) { eye = e; }
	inline void SetTargetVector(const glm::vec3 &t) { target = t; }
	inline void SetUpVector(const glm::vec3 &u) { up = u; } 

	void Update(float deltaTime);

private:
	glm::vec3 eye;
	glm::vec3 target;
	glm::vec3 up;

	std::vector<GenericShader*> shaders;
};

#endif