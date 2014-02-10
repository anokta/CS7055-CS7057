#include "RigidBodyModel.h"

#include "EntityManager.h"

#include "glm\gtx\quaternion.hpp"
#include "glm\gtx\transform.hpp"

using namespace glm;

bool RigidBodyModel::gizmo = false;

RigidBodyModel::RigidBodyModel(RigidBody *b, GenericShader * s, GenericShader * gs)
{
	body = b;
	modelShader = s;
	lineShader = gs;

	switch(body->GetType())
	{
	case RigidBody::BODY_TYPE::BOX:
		modelMesh = MeshLoader::GenerateTexturedCubeMesh("..\\IET\\res\\box.jpg");
		//MeshLoader::LoadMesh("..\\IET\\res\\block.dae", "..\\IET\\res\\block.tga");
		textured = false;
		break;

	case RigidBody::BODY_TYPE::BALL:
		modelMesh = MeshLoader::LoadMesh("..\\IET\\res\\basketball.obj", "..\\IET\\res\\basketball.jpg");		
		textured = true;
		break;

	case RigidBody::BODY_TYPE::ELLIPSOID:
		modelMesh = MeshLoader::GenerateSphereMesh(25);
		textured = false;
		//MeshLoader::LoadMesh("..\\IET\\res\\Mountain Bike.obj");//MeshLoader::GenerateSphereMesh(25);
		break;	

	case RigidBody::BODY_TYPE::PLANE:
		modelMesh = MeshLoader::GeneratePlaneMesh();
		textured = false;
		break;

	case RigidBody::BODY_TYPE::TETRAHEDRON:
		modelMesh = MeshLoader::GenerateTetrahedron();//MeshLoader::GeneratePlaneMesh();
		textured = false;
		break;

	case RigidBody::BODY_TYPE::TRIANGLE:
		modelMesh = MeshLoader::GenerateTriangle();
		textured = false;
		break;

	case RigidBody::BODY_TYPE::CAT:
		modelMesh = MeshLoader::LoadMesh("..\\IET\\res\\cat.obj", "..\\IET\\res\\cat.tga");
		textured = true;
		break;
	}

	modelMesh->SetShader(modelShader);
	body->SetPoints(modelMesh->GetVertices());

	gizmos["BoundingBox"] = MeshLoader::GenerateBoundingBox();
	gizmos["BoundingBox"]->SetShader(lineShader);

	//gizmos["FurthestPoint"] = MeshLoader::GenerateBoundingSphere();
	//gizmos["FurthestPoint"]->SetShader(lineShader);
	gizmos["BetweenLine"] = MeshLoader::GenerateLine(vec4(1,0,1,1));
	gizmos["BetweenLine"]->SetShader(lineShader);

	EntityManager::GetInstance()->AddDrawable(this);
	EntityManager::GetInstance()->AddUpdatable(this);
}

RigidBodyModel::~RigidBodyModel()
{
	EntityManager::GetInstance()->RemoveDrawable(this);	
	EntityManager::GetInstance()->RemoveUpdatable(this);

	delete body;

	delete modelMesh;

	for(auto gizmo : gizmos)
		delete gizmo.second;
	gizmos.clear();
}

void RigidBodyModel::ChangeShader(GenericShader * s)
{
	modelShader = s;

	modelMesh->SetShader(modelShader);
}

void RigidBodyModel::Draw()
{
	// Render object
	mat4 M = modelShader->GetModelMatrix();

	modelShader->SetModelMatrix(M * body->GetTransformationMatrix());
	modelMesh->Render(modelShader);
	modelShader->SetModelMatrix(M);

	// Render gizmos
	if(gizmo)
	{
		gizmos["BetweenLine"]->Render(lineShader);
		//gizmos["FurthestPoint"]->Render(lineShader);

		M = lineShader->GetModelMatrix();

		lineShader->SetModelMatrix(M * body->GetTransformationMatrix());
		gizmos["BoundingBox"]->Render(lineShader);
		lineShader->SetModelMatrix(M);
	}
}

void RigidBodyModel::Update(float deltaTime)
{
	body->Update(deltaTime);
}

bool RigidBodyModel::DetectCollision(RigidBodyModel * rigidBodyModel)
{
	////gizmos["FurthestPoint"]->Translate(furthestA);
	if(body->CheckCollisionBroad(rigidBodyModel->GetBody()))
	{	
		if(gizmoColor.r == 0)
			gizmoColor = vec4(1,1,0,1);
		if(rigidBodyModel->GetGizmoColor().r == 0)
			rigidBodyModel->SetGizmoColor(vec4(1,1,0,1));

		//std::cout << "Broad Collided. . . ";
		vec3 contact = body->CheckCollisionNarrow(rigidBodyModel->GetBody());
		if(contact != vec3(vec3::null))
		{
			//contact = normalize(contact);
			//if(dot(rigidBodyModel->GetBody()->getFurthestPointInDirection(-contact) - body->getFurthestPointInDirection(contact), contact) < 0)
			//	contact = -contact;
			gizmos["BetweenLine"]->SetFromTo(body->getFurthestPointInDirection(normalize(contact)), body->getFurthestPointInDirection(normalize(-contact)));
		
			//gizmos["BetweenLine"]->SetFromTo(body->GetPosition(), body->GetPosition()-normalize(contact));
			//std::cout << "CP: " << contact.x << "\t" << contact.y << "\t" << contact.z << std::endl;
			//std::cout << "NARROW Collided. . . ";
			return true;
		}
	}

	return false;
}