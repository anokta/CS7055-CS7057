#include "XToonMesh.h"
#include "MeshLoader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glm;

XToonMesh::XToonMesh(const std::vector<vec3> &v, const std::vector<GLuint> &i, const std::vector<vec3> &n)
{
	vertices = v;
	indices = i;
	normals = n;

	GenerateBuffer();
}

XToonMesh::~XToonMesh()
{
	glDeleteBuffers(1, &iboID);
	glDeleteTextures( 1, &textureID );
}

void XToonMesh::GenerateBuffer()
{		
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(vec3);
	nSize = normals.size() * sizeof(vec3);

	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize + nSize, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize, nSize, (const GLvoid*)(&normals[0]) );

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Initialize IBO
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), (const GLvoid*)(&indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void XToonMesh::SetShader(GenericShader * shader)
{	
	// Bind the mesh
	glBindVertexArray( vaoID );	
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	//Set up Vertex Arrays  
	GLuint vPositionID = shader->GetVertexPositionID();
	GLuint vNormalID = shader->GetVertexNormalID();

	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vNormalID );
	glVertexAttribPointer( vNormalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

	// Unbind the mesh
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void XToonMesh::Render(GenericShader * shader)
{
	shader->UseProgram();

	shader->BindTexture(textureID);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

	int size;  
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	size /= sizeof(GLuint);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	shader->UnbindTexture();
}


void XToonMesh::ChangeTexture(std::string &texturePath)
{
	glDeleteTextures( 1, &textureID );

	textureID = MeshLoader::loadTexture(texturePath);
}