#include "HelperFunctions.h"

using namespace glm;

quat HelperFunctions::RotationBetweenVectors(vec3 &start, vec3 &dest){
	start = normalize(start);
	dest = normalize(dest);

	// cosine of rotation angle
	float cosAngle = dot(start, dest);

	// rotation axis
	vec3 rotationAxis;

	// if vectors are in opposite direction, guess the axis (since there is no unique answer)
	if (cosAngle < -1 + 0.001f)
	{
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);

		if (length(rotationAxis) < 0.1 )
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		return angleAxis(180.0f, normalize(rotationAxis));
	}
	// rotation angle is zero, don't need to calculate furter
	else if(cosAngle > 0.99f)
		return quat();

	rotationAxis = cross(start, dest);

	// create the rotation quaternion
	float s = sqrt( (1+cosAngle)*2 );

	return quat(
		s * 0.5f, 
		rotationAxis.x * (1/s),
		rotationAxis.y * (1/s),
		rotationAxis.z * (1/s)
		);
}