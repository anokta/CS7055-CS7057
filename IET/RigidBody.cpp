#include "RigidBody.h"

#include "EntityManager.h"

#include "glm\gtx\transform.hpp"
#include "glm\gtx\quaternion.hpp"

#include <algorithm>

using namespace glm;
using namespace std;

RigidBody::RigidBody(vec3 &p, quat &o, vec3 &s)
{
	position = p;
	orientation = o;
	scale = s;

	linearMomentum = vec3();
	force = vec3();

	angularMomentum = vec3();
	torque = vec3();
}


mat4 RigidBody::GetTransformationMatrix()
{
	mat4 T = translate(position);
	mat4 R = toMat4(orientation);
	mat4 S = glm::scale(scale);

	return T * R * S;
}

mat3 RigidBody::GetInertiaInverse() const
{
	return toMat3(orientation) * inverseI * transpose(toMat3(orientation));
}

void RigidBody::SetPoints(const vector<vec3> & vertices)
{
	points.clear();
	for(unsigned int i=0; i<vertices.size(); ++i)
	{
		bool flag = false;
		for(unsigned int j=0; j<points.size(); ++j)
		{
			if(vertices[i] == points[j])
			{
				flag = true;
				break;
			}
		}

		if(!flag)
			points.push_back(vertices[i]); 
	}
}

void RigidBody::ApplyForce(const glm::vec3 &point, const glm::vec3 &f)
{
	force += f;
	torque += cross(point - position, f);
}

bool RigidBody::CheckCollisionBroad(RigidBody * body)
{
	mat3 aOrientation = toMat3(orientation);
	vec3 aX = aOrientation * vec3(1,0,0);
	vec3 aY = aOrientation * vec3(0,1,0);
	vec3 aZ = aOrientation * vec3(0,0,1);

	float aW = scale.x / 2.0f;
	float aH = scale.y / 2.0f;
	float aD = scale.z / 2.0f;

	mat3 bOrientation = toMat3(body->GetOrientation());
	vec3 bX = bOrientation * vec3(1,0,0);
	vec3 bY = bOrientation * vec3(0,1,0);
	vec3 bZ = bOrientation * vec3(0,0,1);

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

RigidBody::Contact * RigidBody::CheckCollisionNarrow(RigidBody * body)
{
	// Support function
	vec3 direction = body->GetPosition() - position;

	vec3 furthestA = getFurthestPointInDirection(direction);
	vec3 furthestB = body->getFurthestPointInDirection(-direction);

	// Minkowski difference
	vec3 mDiff = furthestA - furthestB;

	// Simplex points
	vector<vec3> simplex;
	simplex.push_back(mDiff);

	// Initial direction
	direction = -mDiff;

	int count = 100;
	while(count-- > 0)
	{
		furthestA = getFurthestPointInDirection(direction);
		furthestB = body->getFurthestPointInDirection(-direction);
		mDiff = furthestA - furthestB;

		if(dot(mDiff, direction) < 0)
		{
			return NULL;
		}
		simplex.push_back(mDiff);

		if(checkSimplex(simplex, direction))
		{
			vec3 normal = findContactNormal(body, simplex);
			if(normal != vec3(vec3::null))
			{	
				vector<vec3> contactPoints = findContactPoints(body, normal);
				if(contactPoints.size() != 2)
					return NULL;

				return new Contact(normal, contactPoints[0], contactPoints[1]);
			}

			return NULL;
		}
	}

	cout << "Limit exceeded." << endl;

	return NULL;
}

void RigidBody::RespondCollision(RigidBody *body, vec3 &cpA, vec3 &cpB, vec3 &n)
{
	vec3 rA = cpA - position;
	vec3 rB = cpB - body->GetPosition();

	vec3 J = calculateCollisionImpulse(body, rA, rB, n, 1.0f) * n;

	linearMomentum = linearMomentum + J;
	angularMomentum = angularMomentum + cross(rA, J);

	body->SetLinearMomentum(body->GetLinearMomentum() - J);
	body->SetAngularMomentum(body->GetAngularMomentum() - cross(rB, J));

}

void RigidBody::Update(float deltaTime)
{	
	// linear
	linearMomentum += force * deltaTime;
	position += (linearMomentum * massInverse)  * deltaTime;

	force = vec3();

	// angular
	mat3 currentInverseI = GetInertiaInverse();

	angularMomentum += torque * deltaTime;
	orientation = normalize(toQuat(currentInverseI * toMat3(quat(angularMomentum * deltaTime))) * orientation);

	torque = vec3();
}

bool RigidBody::checkSimplex(vector<vec3> &simplex, vec3 &direction)
{
	vec3 A, B, C, D;

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

		return checkTriangle(simplex, direction);

	case 4:	// tetrahedron
		D = simplex[0];
		C = simplex[1];
		B = simplex[2];
		A = simplex[3];

		vec3 ABC = cross(B-A, C-A);
		vec3 ADB = cross(D-A, B-A);
		vec3 ACD = cross(C-A, D-A);

		if(dot(ABC, -A) > 0)
		{			
			simplex.erase(simplex.begin());

			return checkTriangle(simplex, direction);
		}
		else if(dot(ADB, -A) > 0)
		{
			simplex.erase(simplex.begin() + 1);
			simplex[0] = B;
			simplex[1] = D;

			return checkTriangle(simplex, direction);
		}
		else if(dot(ACD, -A) > 0)
		{
			simplex.erase(simplex.begin() + 2);

			return checkTriangle(simplex, direction);
		}

		return true;
	}
}

bool RigidBody::checkTriangle(vector<glm::vec3> &simplex, glm::vec3 &direction)
{
	vec3 C = simplex[0];
	vec3 B = simplex[1];
	vec3 A = simplex[2];

	// face
	vec3 ABC = cross(B-A, C-A);

	if(dot(cross(ABC, C-A), -A) > 0) // AC plane 
	{
		if(dot(C-A, -A) > 0) // outside AC edge
		{
			direction = cross(cross(C-A, -A), C-A);
			simplex.erase(simplex.begin() + 1);
		}
		else
		{
			if(dot(B-A, -A) > 0) // outside AB edge
			{
				direction = cross(cross(B-A, -A), B-A);
				simplex.erase(simplex.begin());
			}
			else // outside A
			{
				direction = -A;
				simplex.erase(simplex.begin());
				simplex.erase(simplex.begin());
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
				simplex.erase(simplex.begin());
			}
			else // outside A
			{
				direction = -A;
				simplex.erase(simplex.begin());
				simplex.erase(simplex.begin());
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
				simplex[0] = B;
				simplex[1] = C;

				direction = -ABC;
			}
		}
	}

	return false;
}

vector<vec3> RigidBody::findClosestSimplex(vector<vec3> &simplex, vec3 &target)
{
	vec3 D = simplex[0];
	vec3 C = simplex[1];
	vec3 B = simplex[2];
	vec3 A = simplex[3];

	float distances[4];
	distances[0] = dot(-A, normalize(cross(C-A, B-A))); //float dACB = dot(-A, cross(C-A, B-A));
	distances[1] = dot(-A, normalize(cross(B-A, D-A))); //float dABD = dot(-A, cross(B-A, D-A));
	distances[2] = dot(-A, normalize(cross(D-A, C-A))); //float dADC = dot(-A, cross(D-A, C-A));
	distances[3] = dot(-B, normalize(cross(C-B, D-B))); //float dBCD = dot(-A, cross(C-B, D-B));

	int minIndex = 0;
	for(int i=0; i<4; ++i)
		if(distances[i] < distances[minIndex]) 
			minIndex = i;

	switch(minIndex)
	{
	case 0: // ACB
		simplex.erase(simplex.begin());
		simplex[0] = B;
		simplex[1] = C;
		break;

	case 1: // ABD
		simplex.erase(simplex.begin() + 1);
		break;

	case 2: // ADC
		simplex.erase(simplex.begin() + 2);
		simplex[0] = C;
		simplex[1] = D;
		break;

	case 3: // BCD
		simplex.erase(simplex.begin() + 3);
		break;
	}

	findSimplexWithMinDistanceInTriangle(simplex, target);

	return simplex;
}

void RigidBody::findSimplexWithMinDistanceInTriangle(vector<vec3> &simplex, vec3 &target)
{
	vec3 C = simplex[0];
	vec3 B = simplex[1];
	vec3 A = simplex[2];

	vec3 ABC = cross(C-A, B-A);

	if(dot(-A, B-A) <= 0 && dot(-A, C-A) <= 0)
	{
		simplex.erase(simplex.begin());
		simplex.erase(simplex.begin());
	}
	else if(dot(-B, C-B) <= 0 && dot(-B, A-B) <= 0)
	{
		simplex.erase(simplex.begin() + 2);
		simplex.erase(simplex.begin());
	}
	else if(dot(-C, B-C) <= 0 && dot(-C, A-C) <= 0)
	{	
		simplex.erase(simplex.begin() + 2);
		simplex.erase(simplex.begin() + 1);
	}
	else if(dot(cross(ABC, B-A), -A) >= 0 && dot(-A, B-A) >= 0 && dot(-B, A-B) >= 0)
	{
		simplex.erase(simplex.begin());
	}
	else if(dot(cross(ABC, A-C), -A) >= 0 && dot(-C, A-C) >= 0 && dot(-A, C-A) >= 0)
	{
		simplex.erase(simplex.begin() + 1);
	}
	else if(dot(cross(ABC, C-B), -B) >= 0 && dot(-B, C-B) >= 0 && dot(-C, B-C) >= 0)
	{
		simplex.erase(simplex.begin() + 2);
	}
}

vec3 RigidBody::findContactNormal(RigidBody * body, vector<vec3> &simplex)
{
	// EPA //
	vector<Face> faces;
	faces.push_back(Face(0, 1, 2, simplex));
	faces.push_back(Face(0, 1, 3, simplex));
	faces.push_back(Face(0, 2, 3, simplex));
	faces.push_back(Face(1, 2, 3, simplex));

	int count = 100;
	while(count-- > 0)
	{
		Face face = findClosestFace(faces);

		vec3 supportA = getFurthestPointInDirection(face.normal);
		vec3 supportB = body->getFurthestPointInDirection(-face.normal);
		vec3 newPoint = supportA - supportB;

		if(dot(newPoint - face.v1, face.normal) - dot(face.v1, face.normal) < 0.01f) 
		{				
			// Calculate normal
			vec3 result = -face.normal * dot(-face.v1, -face.normal);

			if(result != vec3())
				return normalize(result);
			else
				return vec3(vec3::null);
		}

		// Add new vertex
		simplex.push_back(newPoint);

		// Add relevant faces
		faces.push_back(Face(simplex.size()-1, face.i1, face.i2, simplex));
		faces.push_back(Face(simplex.size()-1, face.i1, face.i3, simplex));
		faces.push_back(Face(simplex.size()-1, face.i2, face.i3, simplex)); 

	}

	return vec3(vec3::null);
}

RigidBody::Face RigidBody::findClosestFace(vector<Face> &faces)
{
	float minDistance = FLT_MAX;
	unsigned int minIndex = 0;

	for(unsigned int i=0; i<faces.size(); ++i)
	{
		float distance = dot(faces[i].v1, faces[i].normal);

		if(distance < minDistance)
		{
			minDistance = distance;
			minIndex = i;
		}
	}

	Face closest = faces[minIndex];
	faces.erase(faces.begin() + minIndex);

	return closest;
}


vector<vec3> RigidBody::findContactPoints(RigidBody * body, glm::vec3 &normal)
{
	vector<vec3> contactPoints;

	vector<vec3> faceA = getFurthestFaceInDirection(-normal);
	vector<vec3> faceB = body->getFurthestFaceInDirection(normal);
	vec3 aNormal = normalize(cross(faceA[0] - faceA[1], faceA[0] - faceA[2]));
	vec3 bNormal = normalize(cross(faceB[0] - faceB[1], faceB[0] - faceB[2]));
	
	vector<vec3> ref, inc;
	vec3 refNormal;
	if(dot(-normal, aNormal) > dot(normal, bNormal)) 
	{
		ref = faceA;
		inc = faceB;

		refNormal = aNormal;
	}
	else
	{
		ref = faceB;
		inc = faceA;

		refNormal = bNormal;
	}

	vector<vec3> planes;
	
	// hack
#pragma region TO_BE_CHANGED

	if(ref.size() == 3)
	{
		planes.push_back(normalize(cross(ref[1]-ref[0], refNormal)));
		planes.push_back(normalize(cross(ref[2]-ref[1], refNormal)));
		planes.push_back(normalize(cross(ref[0]-ref[2], refNormal)));
	}
	else if(ref.size() == 4)
	{
		for(int i=0; i<4; ++i)
		{
			vec3 r = ref[i] - ref[(i+1) % 4];
			if((r.x != 0 && r.y != 0) || (r.x != 0 && r.z != 0) || (r.y != 0 && r.z != 0))
			{
				vec3 tmp = ref[(i+1)%4];
				ref[(i+1)%4] = ref[(i+2)%4];
				ref[(i+2)%4] = tmp;

				break;
			}
		}

		if(dot(cross(ref[1] - ref[0], ref[2] - ref[1]), refNormal) < 0)
		{
			vec3 tmp = ref[1];
			ref[1] = ref[3];
			ref[3] = tmp;
		}
		
		planes.push_back(normalize(cross(ref[1]-ref[0], refNormal)));
		planes.push_back(normalize(cross(ref[2]-ref[1], refNormal)));
		planes.push_back(normalize(cross(ref[3]-ref[2], refNormal)));
		planes.push_back(normalize(cross(ref[0]-ref[3], refNormal)));

		//for(int i=0; i<4; ++i)
		//{
		//	std::cout << "Q2: " << ref[i].x << " " << ref[i].y << " " << ref[i].z << endl;
		//}
	}
#pragma endregion TO_BE_CHANGED

	planes.push_back(refNormal);
	
	for(int i=0; i<inc.size(); ++i)
	{
		contactPoints.push_back(inc[i]);
	}

	for(int i=0; i<planes.size(); ++i)
	{
		for(int j=0; j<contactPoints.size(); ++j)
		{
			float dP = dot(contactPoints[j]-ref[i%ref.size()], planes[i]);
			if(dP > 0)
			{
				vec3 victim = contactPoints[j];
				contactPoints.erase(contactPoints.begin() + j);

				if(contactPoints.size() < 1) return vector<vec3>();
				
				int count = 0;
				vec3 adj = contactPoints[(j - 1) % contactPoints.size()];
				float d = dot(adj - ref[i%ref.size()], -planes[i]);
				if(d > 0)
				{
					contactPoints.insert(contactPoints.begin() + (j - 1) % contactPoints.size() + 1, victim + (dP / (dP + d)) * (adj - victim));
					count++;
				}

				adj = contactPoints[(j + count) % contactPoints.size()];
				d = dot(adj - ref[i%ref.size()], -planes[i]);
				if(d > 0)
				{
					contactPoints.insert(contactPoints.begin() + (j + count) % contactPoints.size(), victim + (dP / (dP + d)) * (adj - victim));
					count++;
				}

				j += count - 1;
			}
		}

	}

	if(contactPoints.size() == 0)
	{
		return vector<vec3>();
	}

	vec3 total = vec3();
	for(int i=0; i<contactPoints.size(); ++i)
		total += contactPoints[i];
	total /= (float)contactPoints.size();

	contactPoints.clear();
	contactPoints.push_back(total);	contactPoints.push_back(total);

	return contactPoints;
}

float RigidBody::calculateCollisionImpulse(RigidBody *body, vec3 &rA, vec3 &rB, vec3 &n, float e)
{
	vec3 pA = GetLinearVelocity() + cross(GetAngularVelocity(), rA);
	vec3 pB = body->GetLinearVelocity() + cross(body->GetAngularVelocity(), rB);
	vec3 relativeV = pA - pB;

	float j = (-(1 + e) * dot(n, relativeV)) / 
		(massInverse + body->GetMassInverse() + dot(n, GetInertiaInverse() * cross(rA, n)) + dot(n, body->GetInertiaInverse() * cross(rB, n)));

	return j;
}

vec3 RigidBody::GetMinDistancePointVeronoi(vec3 &target)
{
	vector<vec3> simplex(points);
	for(int i=0; i<simplex.size(); ++i)
		simplex[i] = vec3(GetTransformationMatrix() * vec4(simplex[i], 1.0f));

	switch(simplex.size())
	{
	case 3: 
		findSimplexWithMinDistanceInTriangle(simplex, target);
		break;
	case 4:
		findClosestSimplex(simplex, target);
		break;
	default:
		return vec3();
	}

	vec3 n;
	switch(simplex.size())
	{
	case 1:
		return simplex[0];
	case 2:
		n = normalize(simplex[1] - simplex[0]);
		return simplex[0] + (dot(target - simplex[0], n) * n);
	case 3:
		n = normalize(cross(simplex[0]-simplex[2], simplex[1]-simplex[2]));
		return target - (dot(target - simplex[0], n) * n);
	}
}

vec3 RigidBody::getFurthestPointInDirection(vec3 &direction)
{
	int furthestPoint = 0;
	float max = FLT_MIN;

	vec3 d = normalize(vec3(toMat4(inverse(orientation)) * vec4(direction, 0.0f))); // toMat3(inverse(orientation)) * direction;
	for(unsigned int i=0; i<points.size(); ++i)
	{
		vec3 vertex = scale * points[i];
		float projection = dot(vertex, d);

		if(projection > max)
		{
			furthestPoint = i;
			max = projection;
		}
	}

	return vec3(GetTransformationMatrix() * vec4(points[furthestPoint], 1.0f));
}

vector<vec3> RigidBody::getFurthestFaceInDirection(vec3 &direction)
{
	vec3 d = normalize(vec3(toMat4(inverse(orientation)) * vec4(direction, 0.0f)));
	
	float max = FLT_MIN;
	int i0 = 0;

	for(unsigned int i=0; i<points.size(); ++i)
	{
		vec3 vertex = scale * points[i];
		float projection = dot(vertex, d);

		if(projection > max)
		{
			i0 = i;
			max = projection;
		}
	}

	max = FLT_MIN;
	int i1 = 0, i2 = 0;
	vec3 point0 = scale * points[i0];

	for(int i=0; i<points.size(); ++i)
	{
		for(int j=i+1; j<points.size(); ++j)
		{
			vec3 point1 = scale * points[i];
			vec3 point2 = scale * points[j];
			
			if(point1 != point0 && point2 != point0)
			{
				vec3 normal = cross(point0 - point1, point0 - point2);
				if(normal != vec3())
				{
					float projection = abs(dot(d, normalize(normal)));

					if(projection > max)
					{
						i1 = i;
						i2 = j;

						max = projection;
					}
				}
			}
		}
	}

	if(dot(d, cross(point0 - scale * points[i1], scale * points[i0] - scale * points[i2])) < 0)
	{
		int temp = i1;
		i1 = i2;
		i2 = temp;
	}

	vector<vec3> face;
	face.push_back(vec3(GetTransformationMatrix() * vec4(points[i0], 1.0f)));
	face.push_back(vec3(GetTransformationMatrix() * vec4(points[i1], 1.0f)));
	face.push_back(vec3(GetTransformationMatrix() * vec4(points[i2], 1.0f)));
	

	for(int i=0; i<points.size(); ++i)
	{
		if(i != i0 && i != i1 && i != i2 && dot(points[i] - points[i0], cross(points[i0] - points[i1], points[i0] - points[i2])) == 0)
		{
			face.push_back(vec3(GetTransformationMatrix() * vec4(points[i], 1.0f)));
		}
	}

	//if(face.size() > 4) cout << "WTFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF?????" << endl;

	return face;
}