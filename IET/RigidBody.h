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
	enum BODY_TYPE { BOX, BALL, ELLIPSOID, MODEL_BODY, PLANE, TERRAIN };
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

	inline void ApplyGravity(const float g) { if(massInverse > 0) force += glm::vec3(0, 1.0f/massInverse, 0) * g; }
	void ApplyForce(const glm::vec3 &point, const glm::vec3 &f);

	void SetPoints(const std::vector<glm::vec3> & vertices);
	
	struct Contact
	{
		Contact(glm::vec3 n, glm::vec3 &a, glm::vec3 &b) : normal(n), cA(a), cB(b) {}

		glm::vec3 normal;
		glm::vec3 cA;
		glm::vec3 cB;
	};

	bool CheckCollisionBroad(RigidBody * body);
	Contact * CheckCollisionNarrow(RigidBody * body);

	void RespondCollision(RigidBody *body, glm::vec3 &cpA, glm::vec3 &cpB, glm::vec3 &n);

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

	float massInverse;
	glm::mat3 inverseI;

	std::vector<glm::vec3> points;
	
	struct Face
	{
		Face(int index1, int index2, int index3, std::vector<glm::vec3> &simplex) : i1(index1), i2(index2), i3(index3){
			v1 = simplex[i1];
			v2 = simplex[i2];
			v3 = simplex[i3];

			normal = glm::normalize(glm::cross(v3 - v1, v2 - v1));
			if(glm::dot(v1, normal) < 0)
				normal = -normal;
		}

		glm::vec3 v1, v2, v3;
		int i1, i2, i3;

		glm::vec3 normal;
	};
	
	std::vector<glm::vec3> findClosestSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());
	void findSimplexWithMinDistanceInTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &target = glm::vec3());

	bool checkSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &direction);
	bool checkTriangle(std::vector<glm::vec3> &simplex, glm::vec3 &direction);

	glm::vec3 findContactNormal(RigidBody * body, std::vector<glm::vec3> &simplex);
	Face findClosestFace(std::vector<Face> &faces);

	std::vector<glm::vec3> findContactPoints(RigidBody * body, glm::vec3 &normal);

	glm::vec3 getFurthestPointInDirection(glm::vec3 &direction);
	std::vector<glm::vec3> getFurthestFaceInDirection(glm::vec3 &direction);

	float calculateCollisionImpulse(RigidBody *body, glm::vec3 &rA, glm::vec3 &rB, glm::vec3 &n, float e = 1.0f);
};

#endif