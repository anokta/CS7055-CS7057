#include "RigidBody.h"

#include "EntityManager.h"

#include "glm\gtx\transform.hpp"
#include "glm\gtx\quaternion.hpp"

#include <algorithm>

using namespace glm;

RigidBody::RigidBody(vec3 &p, quat &o, vec3 &s, float m)
{
	position = p;
	orientation = o;
	scale = s;

	linearMomentum = vec3();
	force = vec3();

	angularMomentum = vec3();
	torque = vec3();

	mass = m;

	collided = false;
}


mat4 RigidBody::GetTransformationMatrix()
{
	mat4 T = translate(position);
	mat4 R = toMat4(orientation);
	mat4 S = glm::scale(scale);

	return T * R * S;
}

bool RigidBody::CheckCollision(RigidBody * body)
{
	if(checkCollisionBroad(body))
	{
		return checkCollisionNarrow(body);
	}

	return false;
}

bool RigidBody::checkCollisionBroad(RigidBody * body)
{
	mat3 aOrientation = toMat3(orientation);
	vec3 aX = normalize(aOrientation * vec3(1,0,0));
	vec3 aY = normalize(aOrientation * vec3(0,1,0));
	vec3 aZ = normalize(aOrientation * vec3(0,0,1));

	float aW = scale.x / 2.0f;
	float aH = scale.y / 2.0f;
	float aD = scale.z / 2.0f;

	mat3 bOrientation = toMat3(body->GetOrientation());
	vec3 bX = normalize(bOrientation * vec3(1,0,0));
	vec3 bY = normalize(bOrientation * vec3(0,1,0));
	vec3 bZ = normalize(bOrientation * vec3(0,0,1));

	vec3 bScale = body->GetScale();
	float bW = bScale.x / 2.0f;
	float bH = bScale.y / 2.0f;
	float bD = bScale.z / 2.0f;

	vec3 T = body->GetPosition() - position;

	// L = Ax
	bool Ax = (dot(T, aX) > (aW + abs(bW * dot(aX, bX)) + abs(bH * dot(aX, bY)) + abs(bD * dot(aX, bZ))));
	if(Ax) return false;
	// L = Ay
	bool Ay = (dot(T, aY) > (aH + abs(bW * dot(aY, bX)) + abs(bH * dot(aY, bY)) + abs(bD * dot(aY, bZ))));
	if(Ay) return false;
	// L = Az
	bool Az = (dot(T, aZ) > (aD + abs(bW * dot(aZ, bX)) + abs(bH * dot(aZ, bY)) + abs(bD * dot(aZ, bZ))));
	if(Az) return false;
	// L = Bx
	bool Bx = (dot(T, bX) > (bW + abs(aW * dot(aX, bX)) + abs(aH * dot(aY, bX)) + abs(aD * dot(aZ, bX))));
	if(Bx) return false;
	// L = By
	bool By = (dot(T, bY) > (bH + abs(aW * dot(aX, bY)) + abs(aH * dot(aY, bY)) + abs(aD * dot(aZ, bY))));
	if(By) return false;
	// L = Bz
	bool Bz = (dot(T, bZ) > (bD + abs(aW * dot(aX, bZ)) + abs(aH * dot(aY, bZ)) + abs(aD * dot(aZ, bZ))));
	if(Bz) return false;

	// L = Ax x Bx
	bool AxBx = (dot(T, cross(aX, bX)) > (abs(aH * dot(aZ, bX)) + abs(aD * dot(aY, bX)) + abs(bH * dot(aX, bZ)) + abs(bD * dot(aX, bY))));
	if(AxBx) return false;
	// L = Ax x By
	bool AxBy = (dot(T, cross(aX, bY)) > (abs(aH * dot(aZ, bY)) + abs(aD * dot(aY, bY)) + abs(bW * dot(aX, bZ)) + abs(bD * dot(aX, bX))));
	if(AxBy) return false;
	// L = Ax x Bz
	bool AxBz = (dot(T, cross(aX, bZ)) > (abs(aH * dot(aZ, bZ)) + abs(aD * dot(aY, bZ)) + abs(bW * dot(aX, bY)) + abs(bH * dot(aX, bX))));
	if(AxBz) return false;
	// L = Ay x Bx
	bool AyBx = (dot(T, cross(aY, bX)) > (abs(aW * dot(aZ, bX)) + abs(aD * dot(aX, bX)) + abs(bH * dot(aY, bZ)) + abs(bD * dot(aY, bY))));
	if(AyBx) return false;
	// L = Ay x By
	bool AyBy = (dot(T, cross(aY, bY)) > (abs(aW * dot(aZ, bY)) + abs(aD * dot(aX, bY)) + abs(bW * dot(aY, bZ)) + abs(bD * dot(aY, bX))));
	if(AyBy) return false;
	// L = Ay x Bz
	bool AyBz = (dot(T, cross(aY, bZ)) > (abs(aW * dot(aZ, bZ)) + abs(aD * dot(aX, bZ)) + abs(bW * dot(aY, bY)) + abs(bH * dot(aY, bX))));
	if(AyBz) return false;
	// L = Az x Bx
	bool AzBx = (dot(T, cross(aZ, bX)) > (abs(aW * dot(aY, bX)) + abs(aH * dot(aX, bX)) + abs(bH * dot(aZ, bZ)) + abs(bD * dot(aZ, bY))));
	if(AzBx) return false;
	// L = Az x By
	bool AzBy = (dot(T, cross(aZ, bY)) > (abs(aW * dot(aY, bY)) + abs(aH * dot(aX, bY)) + abs(bW * dot(aZ, bZ)) + abs(bD * dot(aZ, bX))));
	if(AzBy) return false;
	// L = Az x Bz
	bool AzBz = (dot(T, cross(aZ, bZ)) > (abs(aW * dot(aY, bZ)) + abs(aH * dot(aX, bZ)) + abs(bW * dot(aZ, bY)) + abs(bH * dot(aZ, bX))));
	if(AzBz) return false;

	return true;
}

bool RigidBody::checkCollisionNarrow(RigidBody * body)
{
	// Support function
	vec3 direction = normalize(body->GetPosition() - position); // normalize(body->GetLinearMomentum());

	vec3 furthestA = getFurthestPointInDirection(direction);
	vec3 furthestB = body->getFurthestPointInDirection(-direction);

	// Minkowski difference
	vec3 mDiff = furthestA - furthestB;

	// Simplex points
	std::vector<vec3> simplex;
	simplex.push_back(mDiff);

	// Initial direction
	direction = -mDiff;

	while(true)
	{
		furthestA = getFurthestPointInDirection(direction);
		furthestB = body->getFurthestPointInDirection(-direction);
		mDiff = furthestA - furthestB;

		if(dot(mDiff, direction) < 0)
		{
			std::cout << "No Intersection" << std::endl;
			return false;
		}
		simplex.push_back(mDiff);

		if(checkSimplex(simplex, direction))
		{
			std::cout << "Intersection" << std::endl;
			return true;
		}
	}

	return true;
}

bool RigidBody::checkSimplex(std::vector<vec3> &simplex, vec3 &direction)
{
	vec3 A, B, C, D;
	std::vector<GLuint> indices;

	switch(simplex.size())
	{
	case 2: // line
		B = simplex[0];
		A = simplex[1];

		if(dot(B-A, -A) > 0)
		{
			direction = cross(cross(B-A, -A), B-A);
		}
		else
		{
			direction = -A;
		}

		return false;

	case 3: // triangle
		indices.clear();
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		return checkTriangle(simplex, indices, direction);

		//C = simplex[0];
		//B = simplex[1];
		//A = simplex[2];

		//// face
		//vec3 ABC = cross(B-A, C-A);

		//if(dot(cross(ABC, C-A), -A) > 0) // AC plane 
		//{
		//	if(dot(C-A, -A) > 0) // outside AC edge
		//	{
		//		direction = cross(cross(C-A, -A), C-A);
		//		simplex.erase(simplex.begin() + 1);
		//	}
		//	else
		//	{
		//		if(dot(B-A, -A) > 0) // outside AB edge
		//		{
		//			direction = cross(cross(B-A, -A), B-A);
		//			simplex.erase(simplex.begin());
		//		}
		//		else // outside A
		//		{
		//			direction = -A;
		//			simplex.erase(simplex.begin());
		//			simplex.erase(simplex.begin());
		//		}
		//	}
		//}
		//else // inside AC 
		//{
		//	if(dot(cross(B-A, ABC), -A) > 0) // AB plane 
		//	{
		//		if(dot(B-A, -A) > 0) // outside AB plane
		//		{
		//			direction = cross(cross(B-A, -A), B-A);
		//			simplex.erase(simplex.begin());
		//		}
		//		else // outside A
		//		{
		//			direction = -A;
		//			simplex.erase(simplex.begin());
		//			simplex.erase(simplex.begin());
		//		}
		//	}
		//	else // orthogonal to face
		//	{
		//		if(dot(ABC, -A) > 0) // outside face
		//		{
		//			direction = ABC;
		//		}
		//		else // inside face
		//		{
		//			simplex[0] = B;
		//			simplex[1] = C;

		//			direction = -ABC;
		//		}
		//	}
		//}

		//return false;

	case 4:	// tetrahedron
		D = simplex[0];
		C = simplex[1];
		B = simplex[2];
		A = simplex[3];

		vec3 ABC = cross(B-A, C-A);
		vec3 ABD = cross(B-A, D-A);
		vec3 ACD = cross(C-A, D-A);

		if(dot(ABC, -A) > 0)
		{
			indices.clear();
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(3);

			return checkTriangle(simplex, indices, direction);
		}
		else if(dot(ABD, -A) > 0)
		{
			indices.clear();
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);

			return checkTriangle(simplex, indices, direction);
		}
		else if(dot(ACD, -A) > 0)
		{
			indices.clear();
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(3);

			return checkTriangle(simplex, indices, direction);
		}

		return  dot(cross(C-B, D-B), -A) <= 0;
	}
}

bool RigidBody::checkTriangle(std::vector<glm::vec3> &simplex, std::vector<GLuint> &indices, glm::vec3 &direction)
{
	vec3 C = simplex[indices[0]];
	vec3 B = simplex[indices[1]];
	vec3 A = simplex[indices[2]];

	// face
	vec3 ABC = cross(B-A, C-A);

	if(dot(cross(ABC, C-A), -A) > 0) // AC plane 
	{
		if(dot(C-A, -A) > 0) // outside AC edge
		{
			direction = cross(cross(C-A, -A), C-A);
			std::remove(simplex.begin(), simplex.end(), B);
		}
		else
		{
			if(dot(B-A, -A) > 0) // outside AB edge
			{
				direction = cross(cross(B-A, -A), B-A);
				std::remove(simplex.begin(), simplex.end(), C);
			}
			else // outside A
			{
				direction = -A;
				std::remove(simplex.begin(), simplex.end(), B);
				std::remove(simplex.begin(), simplex.end(), C);
			}
		}
	}
	else // inside AC 
	{
		if(dot(cross(B-A, ABC), -A) > 0) // AB plane 
		{
			if(dot(B-A, -A) > 0) // outside AB plane
			{
				direction = cross(cross(B-A, -A), B-A);
				std::remove(simplex.begin(), simplex.end(), C);
			}
			else // outside A
			{
				direction = -A;
				std::remove(simplex.begin(), simplex.end(), B);
				std::remove(simplex.begin(), simplex.end(), C);
			}
		}
		else // orthogonal to face
		{
			if(dot(ABC, -A) > 0) // outside face
			{
				direction = ABC;
			}
			else // inside face
			{
				simplex[std::find(simplex.begin(), simplex.end(), C) - simplex.begin()] = B;
				simplex[std::find(simplex.begin(), simplex.end(), B) - simplex.begin()] = C;

				direction = -ABC;
			}
		}
	}

	return false;
}

vec3 RigidBody::getFurthestPointInDirection(vec3 &direction)
{
	vec3 furthestPoint;
	float max = FLT_MIN;

	vec3 d = toMat3(inverse(orientation)) * direction;
	for(unsigned int i=0; i<points.size(); ++i)
	{
		vec3 vertex = scale * points[i];
		float projection = dot(vertex, d);

		if(projection > max)
		{
			furthestPoint = vertex;
			max = projection;
		}
	}

	furthestPoint = vec3(GetTransformationMatrix() * vec4(furthestPoint, 1.0f));

	//std::cout << furthestPoint.x << " " << furthestPoint.y << " " << furthestPoint.z << std::endl;

	return furthestPoint;
}

void RigidBody::Update(float deltaTime)
{	
	// linear
	linearMomentum += force * deltaTime;
	position += (linearMomentum/mass)  * deltaTime;

	force = vec3();

	// angular
	mat3 currentInverseI = toMat3(orientation) * inverseI * transpose(toMat3(orientation));

	angularMomentum += torque * deltaTime;
	orientation = normalize(toQuat(currentInverseI * toMat3(quat(angularMomentum * deltaTime))) * orientation);

	torque = vec3();
}