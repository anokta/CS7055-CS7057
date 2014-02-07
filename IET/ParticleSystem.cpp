#include "ParticleSystem.h"

#include "EntityManager.h"
#include "HelperFunctions.h"

#include "glm\gtc\matrix_transform.hpp"

using namespace glm;

#define K_VISCOUS 0.5f
#define GRAVITY 11.5f
#define EQ_FORCE 8.0f

ParticleSystem::ParticleSystem(GenericShader * s, int size)
{
	model = HelperFunctions::GenerateCubeMesh(s);
	shader = s;

	for(int i=0; i<size; ++i)
		particles.push_back(new Particle(vec3(float(rand())/RAND_MAX * 8.0f - 4.0f, float(rand())/RAND_MAX * 2.0f - 1.0f, float(rand())/RAND_MAX * 1.0f - 0.5f)));

	EntityManager::GetInstance()->AddDrawable(this);
}

ParticleSystem::~ParticleSystem()
{
	for(int i=0; i<particles.size(); ++i)
	{
		delete particles[i];
	}

	particles.clear();

	delete model;
}

void ParticleSystem::Draw()
{
	for(int i=0; i<particles.size(); ++i)
	{
		shader->SetModelMatrix(translate(shader->GetModelMatrix(), particles[i]->GetPosition()));
		model->Render();
		shader->SetModelMatrix(translate(shader->GetModelMatrix(), -particles[i]->GetPosition()));
	}
}

void ParticleSystem::ApplyGravity()
{
	ApplyGlobalForce(vec3(0,-1,0) * GRAVITY);
}

void ParticleSystem::ApplyViscousDrag()
{
	for(int i=0; i<particles.size(); ++i)
	{
		particles[i]->AddForce(-K_VISCOUS * particles[i]->GetVelocity());
	}
}

void ParticleSystem::ApplyGlobalForce(vec3 &f)
{
	for(int i=0; i<particles.size(); ++i)
	{
		particles[i]->AddForce(f);
	}
}

void ParticleSystem::ApplyEQForces(float * bands)
{
	for(int i=0; i<particles.size(); ++i)
	{	
		for(int j=0; j<8; ++j)
		{
			float deviation = abs(particles[i]->GetPosition().x - (j - 3.5f));
			float intensity = (bands[j] > 0.42f && deviation <= 0.5f) ? min(8.0f, bands[j]) : 0;// * (particles[i]->GetPosition().y < 0.1f ? 10.0f : 1/particles[i]->GetPosition().y) : 0;
			particles[i]->AddForce(min(6.35f, intensity) * vec3(float(rand())/RAND_MAX * deviation * 4.0f - deviation * 2.0f, EQ_FORCE * (0.75f + float(rand())/RAND_MAX * 0.5f), 0));
		}
	}
}

void ParticleSystem::HandleCollisions(std::vector<ObstaclePlane*> & obstacles)
{
	for(int i=0; i<particles.size(); ++i)
	{
		for(int j=0; j<obstacles.size(); ++j)
		{
			if(particles[i]->IsColliding(obstacles[j]))
			{
				particles[i]->ResolveCollision(obstacles[j]);
			}
		}
	}
}