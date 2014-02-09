#include "Line.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glm;

Line::Line(const std::vector<vec3> &v, const std::vector<vec4> &c)
{
	vertices = v;
	colors = c;

	GenerateBuffer();
}

void Line::GenerateBuffer()
{
	//Initialize VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Calc Array Sizes
	vSize = vertices.size() * sizeof(vec3);
	cSize = colors.size() * sizeof(vec4);

	//Initialize VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vSize + cSize, NULL, GL_DYNAMIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );
	glBufferSubData( GL_ARRAY_BUFFER, vSize, cSize, (const GLvoid*)(&colors[0]) );

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::ChangeColor(vec4 &color)
{	
	for(unsigned int i=0; i<colors.size(); ++i)
		colors[i] = color;

	glBindVertexArray( vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	
	glBufferSubData( GL_ARRAY_BUFFER, vSize, cSize, (const GLvoid*)(&colors[0]) );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Line::SetFromTo(vec3 &from, vec3 &to)
{
	vertices[0] = from;
	vertices[1] = to;

	glBindVertexArray( vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	
	glBufferSubData( GL_ARRAY_BUFFER, 0, vSize, (const GLvoid*)(&vertices[0]) );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Line::SetShader(GenericShader * shader)
{
	// Bind buffer
	glBindVertexArray( vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	//Set up Vertex Arrays  
	GLuint vPositionID = shader->GetVertexPositionID();
	GLuint vColorID = shader->GetVertexColorID();

	glEnableVertexAttribArray( vPositionID );
	glVertexAttribPointer( vPositionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColorID );
	glVertexAttribPointer( vColorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::Render(GenericShader * shader)
{
	shader->UseProgram();

	glBindVertexArray (vaoID);
	glDrawArrays (GL_LINE_LOOP, 0, vertices.size());

	glBindVertexArray(0);
}