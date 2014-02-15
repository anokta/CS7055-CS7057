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
	inline static void ToggleGizmos() { gizmo = !gizmo; }
	
	RigidBodyModel(RigidBody *b, GenericShader * s, GenericShader * gs);
	~RigidBodyModel();

	inline RigidBody * GetBody() { return body; }
	inline glm::vec4 GetGizmoColor() { return gizmoColor; }
	
	void ChangeShader(GenericShader * s);
	inline bool IsTextured() { return textured; }
	
	inline void SetGizmoColor(glm::vec4 & color) { gizmoColor = color; }
	inline void UpdateGizmoColor() { gizmos["BoundingBox"]->ChangeColor(gizmoColor); } //body->IsCollided() ? glm::vec4(1,0,0,1) : glm::vec4(0,1,0,1)); }

	bool ResolveCollision(RigidBodyModel * rigidBodyModel);

	void Draw();
	void Update(float deltaTime);

private:
	static bool gizmo;

	RigidBody * body;

	GenericShader * modelShader;
	GenericShader * lineShader;

	MeshBase * modelMesh;
	std::map<std::string, Line *> gizmos;
	glm::vec4 gizmoColor;
	bool textured;
};

#endif