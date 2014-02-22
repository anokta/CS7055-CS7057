#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <GL/glew.h>
#include <string>

#include "IndexedMesh.h"
#include "TexturedIndexedMesh.h"
#include "BumpedTexturedMesh.h"
#include "Line.h"
#include "ParticleMesh.h"
#include "SkyboxMesh.h"

class MeshLoader
{
public:
	static IndexedMesh * LoadMesh(const std::string &path);
	static TexturedIndexedMesh * LoadMesh(const std::string &path, const std::string &texturePath);
	static BumpedTexturedMesh * LoadBumpedMesh(const std::string &path, const std::string &texturePath, const std::string &normalPath);
	
	static ParticleMesh * GenerateParticleMesh();
	static SimpleMesh * GenerateCubeMesh();
	static SimpleMesh * GenerateSphereMesh(int numSegments);
	static TexturedSimpleMesh * GenerateTexturedCubeMesh(const std::string &texturePath);

	static SimpleMesh * GeneratePlaneMesh();

	static Line * GenerateBoundingBox();
	static Line * GenerateBoundingSphere();
	static Line * GenerateLine(glm::vec4 &color);
	static Line * GenerateTriangle();
	static Line * GenerateTetrahedron();

	static SkyboxMesh * GenerateCubemapMesh(const std::string& PosXFilename,
		const std::string& NegXFilename,
		const std::string& PosYFilename,
		const std::string& NegYFilename,
		const std::string& PosZFilename,
		const std::string& NegZFilename);

private:
	static GLuint loadTexture(const std::string &path);
	static GLuint loadCubemapTexture(
		const std::string& PosXFilename,
		const std::string& NegXFilename,
		const std::string& PosYFilename,
		const std::string& NegYFilename,
		const std::string& PosZFilename,
		const std::string& NegZFilename);

	static void normalizeVertices(std::vector<glm::vec3> &vertices);
};

#endif