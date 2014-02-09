#include "Skybox.h"

#include "MeshLoader.h"

#include "EntityManager.h"

using namespace glm;

Skybox::Skybox(GenericShader * s)
{
	shader = s;

	mesh = MeshLoader::GenerateCubemapMesh("sp3left.png","sp3right.png","sp3top.png","sp3bot.png","sp3front.png","sp3back.png");
	//"rainbow\\rainbleft1.jpg","rainbow\\rainbright1.jpg","rainbow\\rainbtop1.jpg","rainbow\\rainbbot1.jpg","rainbow\\rainbback1.jpg","rainbow\\rainbfront1.jpg");
	mesh->SetShader(shader);

	EntityManager::GetInstance()->AddDrawable(this);
}

Skybox::~Skybox()
{
	EntityManager::GetInstance()->RemoveDrawable(this);

	delete mesh;
}

void Skybox::Draw()
{
	mesh->Render(shader);
}