#ifndef RIGID_BODY_MODEL
#define RIGID_BODY_MODEL

#include <map>

#include "Drawable.h"
#include "Updatable.h"

#include "RigidBody.h"

#include "MeshLoader.h"

class RigidBodyModel : public Drawable, public Updatable
{
public:
	RigidBodyModel(RigidBody *b, GenericShader * s);
	~RigidBodyModel();

	inline RigidBody * GetBody() { return body; }
	
	void ChangeShader(GenericShader * s);
	inline bool IsTextured() { return textured; }

	bool ResolveCollision(RigidBodyModel * rigidBodyModel);

	void Draw();
	void Update(float deltaTime);

private:
	RigidBody * body;

	GenericShader * modelShader;

	MeshBase * modelMesh;
	bool textured;
};

#endif