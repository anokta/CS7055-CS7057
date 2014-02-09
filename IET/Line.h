#ifndef LINE_H
#define LINE_H

#include "MeshBase.h"

class Line : public MeshBase
{
public:
	Line(const std::vector<glm::vec3> &v, const std::vector<glm::vec4> &c);

	void GenerateBuffer();

	void ChangeColor(glm::vec4 &color);
	//void ChangeVertexColor(glm::vec4 &color);
	void SetFromTo(glm::vec3 &f, glm::vec3 &t);

	void SetShader(GenericShader * shader);
	void Render(GenericShader * shader);

private:
	std::vector<glm::vec4> colors;

	GLuint cSize;
};

#endif