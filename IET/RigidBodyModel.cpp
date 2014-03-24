#include "RigidBodyModel.h"

#include "EntityManager.h"

#include "glm\gtx\quaternion.hpp"
#include "glm\gtx\transform.hpp"

using namespace glm;

RigidBodyModel::RigidBodyModel(RigidBody *b, GenericShader * s)
{
	body = b;
	modelShader = s;

	switch(body->GetType())
	{
	case RigidBody::BODY_TYPE::BOX:
		modelMesh = MeshLoader::GenerateTexturedCubeMesh("..\\IET\\res\\box.jpg");
		//MeshLoader::LoadMesh("..\\IET\\res\\block.dae", "..\\IET\\res\\block.tga");
		textured = false;
		break;

	case RigidBody::BODY_TYPE::BALL:
		modelMesh = MeshLoader::LoadBumpedMesh("..\\IET\\res\\Earth.dae", "..\\IET\\res\\Earth_D.tga", "..\\IET\\res\\Earth_N.tga");		
		textured = true;
		break;

	case RigidBody::BODY_TYPE::ELLIPSOID:
		modelMesh = MeshLoader::GenerateSphereMesh(15); //MeshLoader::LoadMesh("..\\IET\\res\\teapot.off");//MeshLoader::GenerateSphereMesh(15);
		textured = false;
		//MeshLoader::LoadMesh("..\\IET\\res\\Mountain Bike.obj");//MeshLoader::GenerateSphereMesh(25);
		break;	

	case RigidBody::BODY_TYPE::PLANE:
		modelMesh = MeshLoader::GenerateCubeMesh();
		textured = false;
		break;	
	
	case RigidBody::BODY_TYPE::TERRAIN:
		modelMesh = MeshLoader::GenerateTerrainMeshXToon("..\\IET\\res\\heightmaps\\heightmap_test.png", body->GetScale().x, body->GetScale().z, body->GetScale().y, "..\\IET\\res\\xtoon\\xtoon_texture_3.png");
		textured = false;
		body->SetScale(vec3(1.0f));

		break;

	case RigidBody::BODY_TYPE::CAT:
		modelMesh = MeshLoader::LoadXToonMesh("..\\IET\\res\\elephal.obj", "..\\IET\\res\\xtoon\\xtoon_texture_2.png");
		textured = true;
		break;
	}

	modelMesh->SetShader(modelShader);
	body->SetPoints(modelMesh->GetVertices());

	EntityManager::GetInstance()->AddDrawable(this);
	EntityManager::GetInstance()->AddUpdatable(this);
}

RigidBodyModel::~RigidBodyModel()
{
	EntityManager::GetInstance()->RemoveDrawable(this);	
	EntityManager::GetInstance()->RemoveUpdatable(this);

	delete body;

	delete modelMesh;
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

}

void RigidBodyModel::Update(float deltaTime)
{
	body->Update(deltaTime);
}

bool RigidBodyModel::ResolveCollision(RigidBodyModel * rigidBodyModel)
{
	if(body->CheckCollisionBroad(rigidBodyModel->GetBody()))
	{	
		RigidBody::Contact * contact = body->CheckCollisionNarrow(rigidBodyModel->GetBody());
		if(contact != NULL)
		{	
			body->RespondCollision(rigidBodyModel->GetBody(), contact->cA, contact->cB, contact->normal);
		
			delete contact;

			return true;
		}
	}

	return false;
}