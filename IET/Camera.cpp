#include "Camera.h"

#include "EntityManager.h"

Camera::Camera(std::vector<GenericShader*> &s, glm::vec3 &e, glm::vec3 &t, glm::vec3 &u) : shaders(s), eye(e), target(t), up(u) 
{
	EntityManager::GetInstance()->AddUpdatable(this);
}

void Camera::Update(float deltaTime)
{
	glm::mat4 V = glm::lookAt(eye, target, up);

	for(unsigned int i=0; i<shaders.size(); ++i)
	{
		// skybox shader
		if(shaders[i]->GetName().compare("Skybox") == 0)
		{
			glm::mat4 Vcopy(V);
			Vcopy[3] = shaders[i]->GetViewMatrix()[3];
			shaders[i]->SetViewMatrix(Vcopy);
		}
		// model shaders
		else
		{
			shaders[i]->SetViewMatrix(V);
			shaders[i]->SetEyeVector(eye);
		}
	}
}