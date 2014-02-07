#ifndef UPDATABLE_H
#define UPDATABLE_H

class Updatable
{
public:
	//Updatable() { EntityManager::GetInstance()->AddUpdatable(this); }

	virtual void Update(float deltaTime) = 0;
};

#endif