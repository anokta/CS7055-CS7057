#include "RigidBodyModel.h"

#include "EntityManager.h"

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
		modelMesh = MeshLoader::LoadMesh("..\\IET\\res\\block.dae", "..\\IET\\res\\block.tga");
		break;

	case RigidBody::BODY_TYPE::BALL:
		modelMesh = MeshLoader::LoadMesh("..\\IET\\res\\basketball.obj", "..\\IET\\res\\basketball.jpg");
		break;

	case RigidBody::BODY_TYPE::ELLIPSOID:
		modelMesh = MeshLoader::GenerateSphereMesh(40);
		break;	
	
	case RigidBody::BODY_TYPE::PLANE:
		modelMesh = MeshLoader::GeneratePlaneMesh();
		break;

	case RigidBody::BODY_TYPE::CAT:
		modelMesh = MeshLoader::LoadMesh("..\\IET\\res\\cat.obj", "..\\IET\\res\\cat.tga");
		break;
	}

	modelMesh->SetShader(modelShader);

	gizmos["BoundingBox"] = MeshLoader::GenerateBoundingBox();
	gizmos["BoundingBox"]->SetShader(lineShader);

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
		M = lineShader->GetModelMatrix();

		lineShader->SetModelMatrix(M * body->GetTransformationMatrix());
		for(auto gizmo : gizmos)
			gizmo.second->Render(lineShader);
		lineShader->SetModelMatrix(M);
	}
}

void RigidBodyModel::Update(float deltaTime)
{
	body->Update(deltaTime);
}