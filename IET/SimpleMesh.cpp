#include "SimpleMesh.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glm;

SimpleMesh::SimpleMesh(const std::vector<vec3> &v, const std::vector<vec4> &c, const std::vector<vec3> &n)
{
	vertices = v;
	colors = c;
	normals = n;
		
	GenerateBuffer();
}

void SimpleMesh::GenerateBuffer()
{
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(vec3);
	cSize = colors.size() * sizeof(vec4);
	nSize = normals.size() * sizeof(vec3);

	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize + cSize + nSize, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize, cSize, (const GLvoid*)(&colors[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize + cSize, nSize, (const GLvoid*)(&normals[0]) );

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SimpleMesh::SetShader(GenericShader * shader)
{	
	// Bind the mesh
	glBindVertexArray( vaoID );	
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	//Set up Vertex Arrays  
	GLuint vPositionID = shader->GetVertexPositionID();
	GLuint vColorID = shader->GetVertexColorID();
	GLuint vNormalID = shader->GetVertexNormalID();

	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
	glEnableVertexAttribArray( vColorID );
	glVertexAttribPointer( vColorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

	glEnableVertexAttribArray( vNormalID );
	glVertexAttribPointer( vNormalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize + cSize));

	// Unbind the mesh
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SimpleMesh::Render(GenericShader * shader)
{
	shader->UseProgram();

	glBindVertexArray (vaoID);
	glDrawArrays (GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
}