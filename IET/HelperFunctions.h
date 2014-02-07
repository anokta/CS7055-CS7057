#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include "glm\glm.hpp"
#include "glm\gtx\quaternion.hpp"

class HelperFunctions
{
public:

	static glm::quat RotationBetweenVectors(glm::vec3 &start, glm::vec3 &dest);
};

#endif