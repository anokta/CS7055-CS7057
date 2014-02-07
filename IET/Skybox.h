#ifndef SKYBOX_H
#define	SKYBOX_H

#include "SkyboxMesh.h"

#include "Drawable.h"

class Skybox : public Drawable
{
public:
    Skybox(GenericShader * s);
	~Skybox();

    void Draw();
    
private:    
	GenericShader * shader;
	
	SkyboxMesh * mesh;
};

#endif
