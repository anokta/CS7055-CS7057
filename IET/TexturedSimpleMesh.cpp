#include "TexturedSimpleMesh.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include "MeshLoader.h"

using namespace glm;

TexturedSimpleMesh::TexturedSimpleMesh(const std::vector<vec3> &v, const std::vector<vec3> &n, const std::vector<vec2> &uv)
{
	vertices = v;
	normals = n;
	uvs = uv;

	GenerateBuffer();
}

void TexturedSimpleMesh::GenerateBuffer()
{
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(vec3);
	nSize = normals.size() * sizeof(vec3);
	uvSize = uvs.size() * sizeof(vec2);

	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize + nSize + uvSize, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize, nSize, (const GLvoid*)(&normals[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize + nSize, uvSize, (const GLvoid*)(&uvs[0]) );

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TexturedSimpleMesh::SetShader(GenericShader * shader)
{	
	// Bind the mesh
	glBindVertexArray( vaoID );	
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	//Set up Vertex Arrays  
	GLuint vPositionID = shader->GetVertexPositionID();
	GLuint vNormalID = shader->GetVertexNormalID();
	GLuint vUvID = shader->GetVertexUVID();

	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vNormalID );
	glVertexAttribPointer( vNormalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

	glEnableVertexAttribArray( vUvID );
	glVertexAttribPointer( vUvID, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize + nSize));

	// Unbind the mesh
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TexturedSimpleMesh::Render(GenericShader * shader)
{
	shader->UseProgram();

	shader->BindTexture(textureID);

	glBindVertexArray (vaoID);
	glDrawArrays (GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	shader->UnbindTexture();
}