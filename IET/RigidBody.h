#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "glm\glm.hpp"

#include "glm\gtc\quaternion.hpp"

#include "SimpleMesh.h"

class RigidBody
{
public:
	RigidBody(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec3 &s = glm::vec3(1,1,1), float m = 1.0f);

	// Type
	enum BODY_TYPE { BOX, BALL, ELLIPSOID, CAT };
	inline BODY_TYPE GetType() { return type; }

	// Getters
	inline glm::vec3 GetPosition() const { return position; }
	inline glm::quat GetOrientation() const { return orientation; }
	inline glm::vec3 GetScale() const { return scale; }

	inline glm::vec3 GetLinearMomentum() const { return linearMomentum; }
	inline glm::vec3 GetAngularMomentum() const { return angularMomentum; }

	inline glm::vec3 GetForce() const { return force; }
	inline glm::vec3 GetTorque() const { return torque; }
	
	// Setters
	inline void SetPosition(const glm::vec3 &p) { position = p; }
	inline void SetOrientation(const glm::quat &o) { orientation = o; }
	inline void SetScale(const glm::vec3 &s) { scale = s; }

	inline void SetLinearMomentum(const glm::vec3 &lm) { linearMomentum = lm; }
	inline void SetAngularMomentum(const glm::vec3 &am) { angularMomentum = am; }

	inline void SetForce(const glm::vec3 &f) { force = f; }
	inline void SetTorque(const glm::vec3 &t) { torque = t; }

	inline void AddForce(const glm::vec3 &f) { force += f; }
	inline void ApplyGravity(float g) { force += glm::vec3(0, mass, 0) * g; }
	
	glm::mat4 GetTransformationMatrix();

	bool CheckCollision(RigidBody * body);

	void Update(float deltaTime);

protected:
	BODY_TYPE type;

	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;

	glm::vec3 linearMomentum;
	glm::vec3 angularMomentum;

	glm::vec3 force;
	glm::vec3 torque;

	float mass;
	glm::mat3 inverseI;
};

#endif