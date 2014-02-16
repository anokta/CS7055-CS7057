#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "glm\glm.hpp"

#include "glm\gtc\quaternion.hpp"

#include "SimpleMesh.h"

#include <map>

class RigidBody
{
public:
	RigidBody(glm::vec3 &p = glm::vec3(), glm::quat &o = glm::quat(), glm::vec3 &s = glm::vec3(1,1,1));

	// Type
	enum BODY_TYPE { BOX, BALL, ELLIPSOID, PLANE, TETRAHEDRON, TRIANGLE, CAT };
	inline BODY_TYPE GetType() { return type; }

	// Getters
	inline glm::vec3 GetPosition() const { return position; }
	inline glm::quat GetOrientation() const { return orientation; }
	inline glm::vec3 GetScale() const { return scale; }

	inline float GetMassInverse() const { return massInverse; } 
	glm::mat3 GetInertiaInverse() const;

	inline float GetMass() const { return (massInverse != 0.0f) ? 1.0f/massInverse : FLT_MAX; }

	inline glm::vec3 GetLinearMomentum() const { return linearMomentum; }
	inline glm::vec3 GetLinearVelocity() const { return linearMomentum * massInverse; }
	inline glm::vec3 GetAngularMomentum() const { return angularMomentum; }
	inline glm::vec3 GetAngularVelocity() const { return GetInertiaInverse() * angularMomentum; }

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

	inline void ApplyGravity(const float g) { force += glm::vec3(0, 1.0f/massInverse, 0) * g; }
	void ApplyForce(const glm::vec3 &point, const glm::vec3 &f);

	void SetPoints(const std::vector<glm::vec3> & vertices);
	
	bool CheckCollisionBroad(RigidBody * body);
	glm::vec3 CheckCollisionNarrow(RigidBody * body);

	glm::vec3 GetMinDistancePointVeronoi(glm::vec3 &target);

	void RespondCollision(RigidBody *body, glm::vec3 &cpA, glm::vec3 &cpB, glm::vec3 &n);

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

	float massInverse;
	glm::mat3 inverseI;

	std::vector<glm::vec3> points;

	// collision
	bool checkSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &direction);
	bool checkTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &direction);

	struct Face
	{
		Face(int i11, int i22, int i33, glm::vec3 v11, glm::vec3 v22, glm::vec3 v33) : i1(i11), i2(i22), i3(i33), v1(v11), v2(v22), v3(v33) {
			normal = glm::normalize(glm::cross(v3 - v1, v2 - v1));
			if(glm::dot(v1, normal) > 0)
				normal = -normal;
		}

		glm::vec3 v1, v2, v3;
		int i1, i2, i3;
		glm::vec3 normal;
	};

	glm::vec3 findContactPoint(std::vector<glm::vec3> &simplex);
	glm::vec3 findContactNormal(std::vector<glm::vec3> &simplex);
	std::vector<glm::vec3> findClosestSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());
	//int findClosestFace(std::vector<glm::vec3> &simplex);
	int findClosestFace(std::vector<Face> &faces);
	void findSimplexWithMinDistanceInTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());
	glm::vec3 getFurthestPointInDirection(glm::vec3 &direction);



	float calculateCollisionImpulse(RigidBody *body, glm::vec3 &rA, glm::vec3 &rB, glm::vec3 &n, float e = 1.0f);
};

#endif