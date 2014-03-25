#include "Image2D.h"

#include "MeshLoader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glm;

Image2D::Image2D(std::string &path, vec2 &p, float s)
{
	position = p;
	size = s;
	
	textureID = MeshLoader::loadTexture(path);

	vertices.push_back(vec3(p.x, p.y, 0.0f));
	vertices.push_back(vec3(p.x + size, p.y, 0.0f));
	vertices.push_back(vec3(p.x, p.y + size, 0.0f));
	vertices.push_back(vec3(p.x, p.y + size, 0.0f));
	vertices.push_back(vec3(p.x + size, p.y, 0.0f));
	vertices.push_back(vec3(p.x + size, p.y + size, 0.0f));
	
	uvs.push_back(vec2(0.0f, 0.0f));
	uvs.push_back(vec2(1.0f, 0.0f));
	uvs.push_back(vec2(0.0f, 1.0f));
	uvs.push_back(vec2(0.0f, 1.0f));
	uvs.push_back(vec2(1.0f, 0.0f));
	uvs.push_back(vec2(1.0f, 1.0f));

	GenerateBuffer();
}

Image2D::~Image2D()
{
	glDeleteTextures(1, &textureID);
}

void Image2D::GenerateBuffer()
{
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(vec3);
	uvSize = uvs.size() * sizeof(vec2);
	
	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize + uvSize, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize, uvSize, (const GLvoid*)(&uvs[0]) );
	
	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Image2D::SetShader(GenericShader * shader)
{	
	// Bind the mesh
	glBindVertexArray( vaoID );	
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	//Set up Vertex Arrays  
	GLuint vPositionID = shader->GetVertexPositionID();
	GLuint vUvID = shader->GetVertexUVID();
	
	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vUvID );
	glVertexAttribPointer( vUvID, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize) );
	
	// Unbind the mesh
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Image2D::Render(GenericShader * shader)
{
	shader->UseProgram();

	shader->BindTexture(textureID);

	glBindVertexArray (vaoID);
	glDrawArrays (GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	shader->UnbindTexture();
}

void Image2D::ChangeTexture(std::string &texturePath)
{
	glDeleteTextures( 1, &textureID );

	textureID = MeshLoader::loadTexture(texturePath);
}