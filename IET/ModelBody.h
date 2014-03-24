#ifndef MODEL_BODY_H
#define MODEL_BODY_H

#include "RigidBody.h"

class ModelBody : public RigidBody
{

public:	
	ModelBody(std::string &modelPath = std::string("..\\IET\\res\\cat.obj"), glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec3 &s = glm::vec3(1,1,1), float d = 1.0f);

	inline std::string GetModelPath() { return path; }

private:
	std::string path;

};

#endif