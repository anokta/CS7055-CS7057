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
	enum BODY_TYPE { BOX, BALL, ELLIPSOID, PLANE, TETRAHEDRON, TRIANGLE, CAT };
	inline BODY_TYPE GetType() { return type; }

	// Getters
	inline glm::vec3 GetPosition() const { return position; }
	inline glm::quat GetOrientation() const { return orientation; }
	inline glm::vec3 GetScale() const { return scale; }

	inline float GetMass() const { return mass; }

	inline glm::vec3 GetLinearMomentum() const { return linearMomentum; }
	inline glm::vec3 GetAngularMomentum() const { return angularMomentum; }

	inline glm::vec3 GetForce() const { return force; }
	inline glm::vec3 GetTorque() const { return torque; }

	inline std::vector<glm::vec3> GetPoints() { return points; }

	glm::mat4 GetTransformationMatrix();
	
	// Setters
	inline void SetPosition(const glm::vec3 &p) { position = p; }
	inline void SetOrientation(const glm::quat &o) { orientation = o; }
	inline void SetScale(const glm::vec3 &s) { scale = s; }

	inline void SetLinearMomentum(const glm::vec3 &lm) { linearMomentum = lm; }
	inline void SetAngularMomentum(const glm::vec3 &am) { angularMomentum = am; }

	inline void SetForce(const glm::vec3 &f) { force = f; }
	inline void SetTorque(const glm::vec3 &t) { torque = t; }

	inline void AddForce(const glm::vec3 &f) { force += f; }
	inline void ApplyGravity(const float g) { force += glm::vec3(0, mass, 0) * g; }

	void SetPoints(const std::vector<glm::vec3> & vertices);
	
	bool CheckCollisionBroad(RigidBody * body);
	bool CheckCollisionNarrow(RigidBody * body);

	glm::vec3 GetMinDistancePointVeronoi(glm::vec3 &target);

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

	std::vector<glm::vec3> points;

	// collision
	bool checkSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &direction);
	bool checkTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &direction);
	glm::vec3 getFurthestPointInDirection(glm::vec3 &direction);

	// contact model
	std::vector<glm::vec3> findContactPoints(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());
	void findSimplexWithMinDistanceInTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());
};

#endif