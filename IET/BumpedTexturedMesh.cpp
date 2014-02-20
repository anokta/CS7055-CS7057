#include "BumpedTexturedMesh.h"

using namespace glm;

BumpedTexturedMesh::BumpedTexturedMesh(const std::vector<vec3> &v, const std::vector<GLuint> &i, const std::vector<vec3> &n, const std::vector<vec2> &uv, const std::vector<vec4> &t) : TexturedIndexedMesh(v, i, n, uv)
{
	tangents = t;

	GenerateBuffer();
}

void BumpedTexturedMesh::GenerateBuffer()
{
	glBindVertexArray( vaoID );
	
	glGenBuffers (1, &tangentVboID);
	glBindBuffer (GL_ARRAY_BUFFER, tangentVboID);

	glBufferData (GL_ARRAY_BUFFER, tangents.size() * sizeof(vec4), &tangents[0], GL_STATIC_DRAW);
	
	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BumpedTexturedMesh::SetShader(GenericShader * shader)
{
	TexturedIndexedMesh::SetShader(shader);

	// Bind the mesh
	glBindVertexArray( vaoID );	
	glBindBuffer( GL_ARRAY_BUFFER, tangentVboID );

	GLuint vTangentID = shader->GetVertexTangentID();
	
	glEnableVertexAttribArray( vTangentID );
	glVertexAttribPointer (vTangentID, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
//
void BumpedTexturedMesh::Render(GenericShader * shader)
{
	shader->UseProgram();
	
	shader->BindTexture(textureID,GenericShader::TextureType::DIFFUSE);
	shader->BindTexture(normalTextureID, GenericShader::TextureType::NORMAL);

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