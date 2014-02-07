#include "EntityManager.h"

EntityManager * EntityManager::_instance = NULL;

/**
* Create the unique instance.
*/
void EntityManager::Create() { 
	if(_instance == NULL){
		_instance = new EntityManager();
	}
}

/**
* Destroy the unique instance.
*/
void EntityManager::Destroy() {
	delete _instance;
	_instance = NULL;
}

EntityManager::~EntityManager()
{
	DeleteAllEntities();
}

void EntityManager::DrawEntities()
{
	for(unsigned int i=0; i<drawables.size(); ++i)
	{
		drawables[i]->Draw();
	}
}

void EntityManager::UpdateEntities(float deltaTime)
{
	for(unsigned int i=0; i<updatables.size(); ++i)
	{
		updatables[i]->Update(deltaTime);
	}
}

void EntityManager::RemoveDrawable(Drawable * drawable) { 
	for(unsigned int i=0; i<drawables.size(); ++i)
		if(drawable == drawables[i])
			drawables.erase(drawables.begin() + i); 
}

void EntityManager::RemoveUpdatable(Updatable * updatable) { 		
	for(unsigned int i=0; i<updatables.size(); ++i)
		if(updatable == updatables[i])
			updatables.erase(updatables.begin() + i); 
}

void EntityManager::DeleteAllEntities()
{
	for(unsigned int i=0; i<drawables.size(); ++i)
	{
		if(drawables[i] != NULL)
			delete drawables[i];
	}

	drawables.clear();

	for(unsigned int i=0; i<updatables.size(); ++i)
	{
		if(updatables[i] != NULL)
			delete updatables[i];
	}

	updatables.clear();
}