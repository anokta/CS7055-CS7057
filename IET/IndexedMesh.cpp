#include "IndexedMesh.h"

using namespace glm;

IndexedMesh::IndexedMesh(const std::vector<vec3> &v, const std::vector<GLuint> &i, const std::vector<vec4> &c, const std::vector<vec3> &n) : SimpleMesh(v, c, n)
{
	indices = i;

	GenerateBuffer();
}

IndexedMesh::~IndexedMesh()
{
	glDeleteBuffers(1, &iboID);
}

void IndexedMesh::GenerateBuffer()
{	
	//Initialize IBO
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), (const GLvoid*)(&indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void IndexedMesh::Render(GenericShader * shader)
{
	shader->UseProgram();

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

	int size;  
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	size /= sizeof(GLuint);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}