#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <vector>

#include "Drawable.h"
#include "Updatable.h"

class EntityManager 
{
public:	
	// Maintain the manager
	inline static EntityManager * GetInstance() { return _instance; }
	static void Create(); 
	static void Destroy();
	
	~EntityManager();

	// Manage entities
	inline void AddDrawable(Drawable * drawable) { drawables.push_back(drawable); }
	inline void AddUpdatable(Updatable * updatable) { updatables.push_back(updatable); }

	void RemoveDrawable(Drawable * drawable);
	void RemoveUpdatable(Updatable * updatable);

	void DeleteAllEntities();


	void DrawEntities();
	void UpdateEntities(float timeDelta);

private:
	static EntityManager * _instance;

	std::vector<Drawable*> drawables;
	std::vector<Updatable*> updatables;
};

#endif