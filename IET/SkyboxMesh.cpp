#include "SkyboxMesh.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glm;

SkyboxMesh::SkyboxMesh(const std::vector<vec3> &v)
{
	vertices = v;

	GenerateBuffer();
}

void SkyboxMesh::GenerateBuffer()
{
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(glm::vec3);

	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]));

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SkyboxMesh::SetShader(GenericShader * shader)
{	
	// Bind
	glBindVertexArray( vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	GLuint vPositionID = shader->GetVertexPositionID();

	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SkyboxMesh::Render(GenericShader * shader)
{
	shader->UseProgram();

	glDepthMask (GL_FALSE);

	shader->BindCubeTexture(textureID);
		
	glBindVertexArray (vaoID);
	glDrawArrays (GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
	
	shader->UnbindCubeTexture();

	glDepthMask (GL_TRUE);
}