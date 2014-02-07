#include "Camera.h"

#include "EntityManager.h"

Camera::Camera(std::vector<GenericShader*> &s, glm::vec3 &e, glm::vec3 &t, glm::vec3 &u) : shaders(s), eye(e), target(t), up(u) 
{
	EntityManager::GetInstance()->AddUpdatable(this);
}

void Camera::Update(float deltaTime)
{
	for(unsigned int i=0; i<shaders.size(); ++i)
	{
		shaders[i]->SetViewMatrix(glm::lookAt(eye, target, up));
		shaders[i]->SetEyeVector(eye);
	}
}