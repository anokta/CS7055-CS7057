#ifndef MESH_BASE
#define MESH_BASE

#include <vector>

#include "GenericShader.h"

class MeshBase
{
public:
	~MeshBase() 
	{ 
		glDeleteBuffers(1, &vboID);
		glDeleteVertexArrays(1, &vaoID);
	}

	inline std::vector<glm::vec3> GetVertices() { return vertices; }

	virtual void GenerateBuffer() = 0;
	
	virtual void SetShader(GenericShader * shader) = 0;
	virtual void Render(GenericShader * shader) = 0;
	
protected:
	GLuint vboID, vaoID;
	
	std::vector<glm::vec3> vertices;
	GLuint vSize;
};

#endif