#ifndef RIGID_BODY_MODEL
#define RIGID_BODY_MODEL

#include "Drawable.h"
#include "Updatable.h"

#include "RigidBody.h"

#include "MeshLoader.h"

class RigidBodyModel : public Drawable, public Updatable
{
public:
	inline static void ToggleGizmos() { gizmo = !gizmo; }
	
	RigidBodyModel(RigidBody *b, GenericShader * s, GenericShader * gs);
	~RigidBodyModel();

	inline RigidBody * GetBody() { return body; }
	
	void ChangeShader(GenericShader * s);

	inline void ChangeColor(glm::vec4 & color) { boundingBox->ChangeColor(color); }

	void Draw();
	void Update(float deltaTime);

private:
	static bool gizmo;

	RigidBody * body;

	GenericShader * modelShader;
	GenericShader * lineShader;

	MeshBase * modelMesh;
	Line * boundingBox;
};

#endif