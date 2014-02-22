#include "MeshLoader.h"

#include "Assimp\include\assimp\Scene.h"
#include "Assimp\include\assimp\Importer.hpp"
#include "Assimp\include\assimp\PostProcess.h"

#include "SOIL\include\SOIL.h"

#include "glm\ext.hpp"

using namespace glm;
using namespace std;


IndexedMesh * MeshLoader::LoadMesh(const std::string &path)
{
	// Load the file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, 
		aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeGraph
		| aiProcess_OptimizeMeshes
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_GenSmoothNormals
		);

	if (scene->HasMeshes())
	{
		// Get the model mesh
		aiMesh &mesh = *scene->mMeshes[0];

		if (!mesh.HasPositions() || !mesh.HasFaces()) 
			return NULL;

		// Initialize the model
		vector<glm::vec3> vertices;
		vector<GLuint> indices;
		vector<glm::vec4> colors;
		vector<glm::vec3> normals;

		// Get mesh properties 
		for (unsigned int i=0; i<mesh.mNumVertices; ++i) 
		{
			// Get vertices
			vertices.push_back(glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z));

			// Get colors
			if(mesh.HasVertexColors(0))
			{
				colors.push_back(glm::vec4(mesh.mColors[0][i].r, mesh.mColors[0][i].g, mesh.mColors[0][i].b, mesh.mColors[0][i].a));
			}
			else
			{
				colors.push_back(glm::vec4(1,1,1,1));
			}

			// Get normals
			normals.push_back(glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z));
		}

		// Normalize vertices
		normalizeVertices(vertices);

		// Get indices
		for (unsigned int i=0; i<mesh.mNumFaces; ++i) 
		{
			aiFace face = mesh.mFaces[i];

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		return new IndexedMesh(vertices, indices, colors, normals);
	}

	return NULL;
}

TexturedIndexedMesh * MeshLoader::LoadMesh(const std::string &path, const string &texturePath)
{
	// Load the file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, 
		aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeGraph
		| aiProcess_OptimizeMeshes
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_GenSmoothNormals
		);

	if (scene->HasMeshes())
	{
		// Get the model mesh
		aiMesh &mesh = *scene->mMeshes[0];

		if (!mesh.HasPositions() || !mesh.HasFaces()) 
			return NULL;

		// Initialize the model
		vector<glm::vec3> vertices;
		vector<GLuint> indices;
		vector<glm::vec3> normals;
		vector<glm::vec2> uvs; 

		// Get mesh properties 
		for (unsigned int i=0; i<mesh.mNumVertices; ++i) 
		{
			// Get vertices
			vertices.push_back(glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z));
			//vertices[vertices.size()-1] /= 38;

			// Get normals
			normals.push_back(glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z));

			// Get UVs
			uvs.push_back(glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y));
		}		

		// Normalize vertices
		normalizeVertices(vertices);

		// Get indices
		for (unsigned int i=0; i<mesh.mNumFaces; ++i) 
		{
			aiFace face = mesh.mFaces[i];

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		TexturedIndexedMesh * modelMesh =  new TexturedIndexedMesh(vertices, indices, normals, uvs);
		modelMesh->SetTexture(loadTexture(texturePath));

		return modelMesh;
	}

	return NULL;
}

BumpedTexturedMesh * MeshLoader::LoadBumpedMesh(const std::string &path, const string &texturePath, const string &normalPath)
{
	// Load the file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, 
		aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeGraph
		| aiProcess_OptimizeMeshes
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		);

	if (scene->HasMeshes())
	{
		// Get the model mesh
		aiMesh &mesh = *scene->mMeshes[0];

		if (!mesh.HasPositions() || !mesh.HasFaces()) 
			return NULL;

		// Initialize the model
		vector<glm::vec3> vertices;
		vector<GLuint> indices;
		vector<glm::vec3> normals;
		vector<glm::vec2> uvs;
		vector<glm::vec4> tangents;

		// Get mesh properties 
		for (unsigned int i=0; i<mesh.mNumVertices; ++i) 
		{
			// Get vertices
			vertices.push_back(glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z));
			//vertices[vertices.size()-1] /= 38;

			// Get normals
			normals.push_back(glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z));

			// Get UVs
			uvs.push_back(glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y));

			if (mesh.HasTangentsAndBitangents()) 
			{
				const aiVector3D tangent = mesh.mTangents[i];
				const aiVector3D bitangent = mesh.mBitangents[i];

				// put the three vectors into my vec3 struct format for doing maths
				vec3 t (tangent.x, tangent.y, tangent.z);
				vec3 b (bitangent.x, bitangent.y, bitangent.z);
				// orthogonalise and normalise the tangent so we can use it in something
				// approximating a T,N,B inverse matrix
				vec3 t_i = t - normals[i] * dot (normals[i], t);
				//5std::cout << i << ": " << t_i.x << " " << t_i.y << " " << t_i.z << std::endl;
				if(!isnan(t_i.x)) t_i = normalize(t_i);

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = (dot (cross (normals[i], t), b));
				if (det < 0.0f) {
					det = -1.0f;
				} else {
					det = 1.0f;
				}

				tangents.push_back(vec4(t_i, det));
			}
		}		

		// Normalize vertices
		normalizeVertices(vertices);

		// Get indices
		for (unsigned int i=0; i<mesh.mNumFaces; ++i) 
		{
			aiFace face = mesh.mFaces[i];

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		BumpedTexturedMesh * modelMesh =  new BumpedTexturedMesh(vertices, indices, normals, uvs, tangents);
		modelMesh->SetTexture(loadTexture(texturePath));
		modelMesh->SetNormalTexture(loadTexture(normalPath));

		return modelMesh;
	}

	return NULL;
}

ParticleMesh * MeshLoader::GenerateParticleMesh()
{
	vec3 vertices[] = {
		vec3(-0.025f,-0.025f,-0.025f), // triangle 1 : begin
		vec3(-0.025f,-0.025f, 0.025f),
		vec3(-0.025f, 0.025f, 0.025f), // triangle 1 : end
		vec3(0.025f, 0.025f,-0.025f), // triangle 2 : begin
		vec3(-0.025f,-0.025f,-0.025f),
		vec3(-0.025f, 0.025f,-0.025f), // triangle 2 : end
		vec3(0.025f,-0.025f, 0.025f),
		vec3(-0.025f,-0.025f,-0.025f),
		vec3(0.025f,-0.025f,-0.025f),
		vec3(0.025f, 0.025f,-0.025f),
		vec3(0.025f,-0.025f,-0.025f),
		vec3(-0.025f,-0.025f,-0.025f),
		vec3(-0.025f,-0.025f,-0.025f),
		vec3(-0.025f, 0.025f, 0.025f),
		vec3(-0.025f, 0.025f,-0.025f),
		vec3( 0.025f,-0.025f, 0.025f),
		vec3(-0.025f,-0.025f, 0.025f),
		vec3(-0.025f,-0.025f,-0.025f),
		vec3(-0.025f, 0.025f, 0.025f),
		vec3(-0.025f,-0.025f, 0.025f),
		vec3(0.025f,-0.025f, 0.025f),
		vec3(0.025f, 0.025f, 0.025f),
		vec3(0.025f,-0.025f,-0.025f),
		vec3(0.025f, 0.025f,-0.025f),
		vec3(0.025f,-0.025f,-0.025f),
		vec3(0.025f, 0.025f, 0.025f),
		vec3(0.025f,-0.025f, 0.025f),
		vec3(0.025f, 0.025f, 0.025f),
		vec3(0.025f, 0.025f,-0.025f),
		vec3(-0.025f, 0.025f,-0.025f),
		vec3(0.025f, 0.025f, 0.025f),
		vec3(-0.025f, 0.025f,-0.025f),
		vec3(-0.025f, 0.025f, 0.025f),
		vec3(0.025f, 0.025f, 0.025f),
		vec3(-0.025f, 0.025f, 0.025f),
		vec3(0.025f,-0.025f, 0.025f)
	};
	
	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	return new ParticleMesh(v);
}

SimpleMesh * MeshLoader::GenerateCubeMesh()
{
	vec3 vertices[] = {
		vec3(-0.5f,-0.5f,-0.5f), // triangle 1 : begin
		vec3(-0.5f,-0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f), // triangle 1 : end
		vec3(0.5f, 0.5f,-0.5f), // triangle 2 : begin
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f, 0.5f,-0.5f), // triangle 2 : end
		vec3(0.5f,-0.5f, 0.5f),
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(0.5f,-0.5f,-0.5f),
		vec3(0.5f, 0.5f,-0.5f),
		vec3(0.5f,-0.5f,-0.5f),
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f,-0.5f),
		vec3( 0.5f,-0.5f, 0.5f),
		vec3(-0.5f,-0.5f, 0.5f),
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f,-0.5f, 0.5f),
		vec3(0.5f,-0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(0.5f,-0.5f,-0.5f),
		vec3(0.5f, 0.5f,-0.5f),
		vec3(0.5f,-0.5f,-0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(0.5f,-0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f,-0.5f),
		vec3(-0.5f, 0.5f,-0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f,-0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f,-0.5f, 0.5f)
	};

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	vec4 colors[] = 
	{
		vec4(0.583f,  0.771f,  0.014f, 1.0f),
		vec4(0.609f,  0.115f,  0.436f, 1.0f),
		vec4(0.327f,  0.483f,  0.844f, 1.0f),
		vec4(0.822f,  0.569f,  0.201f, 1.0f),
		vec4(0.435f,  0.602f,  0.223f, 1.0f),
		vec4(0.310f,  0.747f,  0.185f, 1.0f),
		vec4(0.597f,  0.770f,  0.761f, 1.0f),
		vec4(0.559f,  0.436f,  0.730f, 1.0f),
		vec4(0.359f,  0.583f,  0.152f, 1.0f),
		vec4(0.483f,  0.596f,  0.789f, 1.0f),
		vec4(0.559f,  0.861f,  0.639f, 1.0f),
		vec4(0.195f,  0.548f,  0.859f, 1.0f),
		vec4(0.014f,  0.184f,  0.576f, 1.0f),
		vec4(0.771f,  0.328f,  0.970f, 1.0f),
		vec4(0.406f,  0.615f,  0.116f, 1.0f),
		vec4(0.676f,  0.977f,  0.133f, 1.0f),
		vec4(0.971f,  0.572f,  0.833f, 1.0f),
		vec4(0.140f,  0.616f,  0.489f, 1.0f),
		vec4(0.997f,  0.513f,  0.064f, 1.0f),
		vec4(0.945f,  0.719f,  0.592f, 1.0f),
		vec4(0.543f,  0.021f,  0.978f, 1.0f),
		vec4(0.279f,  0.317f,  0.505f, 1.0f),
		vec4(0.167f,  0.620f,  0.077f, 1.0f),
		vec4(0.347f,  0.857f,  0.137f, 1.0f),
		vec4(0.055f,  0.953f,  0.042f, 1.0f),
		vec4(0.714f,  0.505f,  0.345f, 1.0f),
		vec4(0.783f,  0.290f,  0.734f, 1.0f),
		vec4(0.722f,  0.645f,  0.174f, 1.0f),
		vec4(0.302f,  0.455f,  0.848f, 1.0f),
		vec4(0.225f,  0.587f,  0.040f, 1.0f),
		vec4(0.517f,  0.713f,  0.338f, 1.0f),
		vec4(0.053f,  0.959f,  0.120f, 1.0f),
		vec4(0.393f,  0.621f,  0.362f, 1.0f),
		vec4(0.673f,  0.211f,  0.457f, 1.0f),
		vec4(0.820f,  0.883f,  0.371f, 1.0f),
		vec4(0.982f,  0.099f,  0.879f, 1.0f)
	};

	std::vector<vec4> c(std::begin(colors), std::end(colors));

	c.clear();

	vec4 color = vec4(float(rand())/RAND_MAX * 0.5f + 0.5f, float(rand())/RAND_MAX * 0.5f + 0.5f, float(rand())/RAND_MAX * 0.5f + 0.5f, 1.0f);
	for(unsigned int i=0; i<v.size(); ++i)
		c.push_back(color);

	// Calculate Normals
	std::vector<vec3> n;
	for(unsigned int i=0; i<v.size(); i+=3)
	{
		vec3 v1 = v[i+1] - v[i];
		vec3 v2 = v[i+2] - v[i];

		vec3 norm = normalize(cross(v1, v2));
		n.push_back(norm);
		n.push_back(norm);
		n.push_back(norm);
	}

	return new SimpleMesh(v, c, n);
}

SimpleMesh * MeshLoader::GenerateSphereMesh(int numSegments)
{
	std::vector< vec3 > v;
	std::vector< vec3 > n;

	std::vector<vec3> points;
	for (int j= 0; j < numSegments; ++j)
	{
		float theta = (pi<float>()*j)/(numSegments);

		for( int i=0; i<numSegments; ++i)
		{
			float phi = (2*pi<float>()*i)/(numSegments);
			points.push_back(vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)) / 2.0f);
		}
	}

	for (int j= 0; j < numSegments; j ++)
	{ 

		for( int i=0; i<numSegments; i++ )
		{
			int iNext = i+1;
			if (i == numSegments - 1)
				iNext = 0;

			int index = (j*numSegments*6)+(i*6);

			v.push_back(points[j*numSegments+i]);
			v.push_back(points[j*numSegments+iNext]);

			if (j != numSegments -1)
				v.push_back(points[((j+1)*numSegments)+i]);
			else
				v.push_back(vec3( 0, 0, -0.5f));

			n.push_back(v[index]);
			n.push_back(v[index+1]);
			n.push_back(v[index+2]);

			v.push_back(v[index+2]);
			v.push_back(v[index+1]);

			if (j != numSegments - 1)
				v.push_back(points[((j+1)*numSegments)+iNext]);
			else
				v.push_back(vec3( 0,0,-0.5f));

			n.push_back(v[index+3]);
			n.push_back(v[index+4]);
			n.push_back(v[index+5]);
		}
	}

	std::vector<vec4> c;
	vec4 color = vec4(float(rand())/RAND_MAX * 0.5f + 0.5f, float(rand())/RAND_MAX * 0.5f + 0.5f, float(rand())/RAND_MAX * 0.5f + 0.5f, 1.0f);
	for(unsigned int i=0; i<v.size(); ++i)
		c.push_back(color);

	return new SimpleMesh(v, c, n);
}

TexturedSimpleMesh * MeshLoader::GenerateTexturedCubeMesh(const string &texturePath)
{
	vec3 vertices[] = {		
		// x
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f), 

		vec3(0.5f,-0.5f,-0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(0.5f, 0.5f, 0.5f), 

		// y
		vec3(-0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, 0.5f), 

		vec3(-0.5f, 0.5f, -0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),

		// z
		vec3(-0.5f, -0.5f,-0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(0.5f, 0.5f, -0.5f), 

		vec3(-0.5f, -0.5f, 0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f)
	};	

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	// Texture coordinates
	vec2 uvs[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),

		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),

		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),

		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),

		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f)
	};

	std::vector<vec2> uv(std::begin(uvs), std::end(uvs));

	// Calculate Normals
	std::vector<vec3> n;
	for(unsigned int i=0; i<v.size(); i+=3)
	{
		vec3 v1 = v[i+1] - v[i];
		vec3 v2 = v[i+2] - v[i];

		vec3 norm = normalize(cross(v1, v2));
		n.push_back(norm);
		n.push_back(norm);
		n.push_back(norm);
	}

	TexturedSimpleMesh * cube = new TexturedSimpleMesh(v, n, uv);
	cube->SetTexture(loadTexture(texturePath));

	return cube;
}

SimpleMesh * MeshLoader::GeneratePlaneMesh()
{
	vec3 vertices[] =
	{
		vec3(-0.5f, 0.0f, -0.5f),
		vec3(-0.5f, 0.0f, 0.5f),
		vec3(0.5f, 0.0f, -0.5f),
		vec3(0.5f, 0.0f, -0.5f),
		vec3(-0.5f, 0.0f, 0.5f),
		vec3(0.5f, 0.0f, 0.5f),
	};

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	std::vector<vec4> c;
	for(unsigned int i=0; i<v.size(); ++i)
		c.push_back(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	// Calculate Normals
	std::vector<vec3> n;
	for(unsigned int i=0; i<v.size(); i+=3)
	{
		vec3 v1 = v[i+1] - v[i];
		vec3 v2 = v[i+2] - v[i];

		vec3 norm = normalize(cross(v1, v2));
		n.push_back(norm);
		n.push_back(norm);
		n.push_back(norm);
	}

	return new SimpleMesh(v, c, n);
}

Line * MeshLoader::GenerateBoundingBox()
{
	vec3 vertices[] = 
	{
		vec3(-0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(-0.5f, -0.5f, -0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f, -0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f)
	};

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	std::vector<vec4> c;
	for(unsigned int i=0; i<v.size(); ++i)
		c.push_back(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	return new Line(v, c);
}

Line * MeshLoader::GenerateBoundingSphere()
{
	int numSegments = 40;

	std::vector< vec3 > v;

	std::vector<vec3> points;
	for (int j= 0; j < numSegments; ++j)
	{
		float theta = (pi<float>()*j)/(numSegments);

		for( int i=0; i<numSegments; ++i)
		{
			float phi = (2*pi<float>()*i)/(numSegments);
			points.push_back(vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)) / 20.0f);
		}
	}

	for (int j= 0; j < numSegments; j ++)
	{ 

		for( int i=0; i<numSegments; i++ )
		{
			int iNext = i+1;
			if (i == numSegments - 1)
				iNext = 0;

			int index = (j*numSegments*6)+(i*6);

			v.push_back(points[j*numSegments+i]);
			v.push_back(points[j*numSegments+iNext]);

			if (j != numSegments -1)
				v.push_back(points[((j+1)*numSegments)+i]);
			else
				v.push_back(vec3( 0, 0, -0.05f));

			v.push_back(v[index+2]);
			v.push_back(v[index+1]);

			if (j != numSegments - 1)
				v.push_back(points[((j+1)*numSegments)+iNext]);
			else
				v.push_back(vec3( 0,0,-0.05f));
		}
	}

	//for(int i=0; i< v.size(); ++i)
	//	v[i] += offset;

	std::vector<vec4> c;
	for(unsigned int i=0; i<v.size(); ++i)
		c.push_back(vec4(0,0,1,1));

	return new Line(v, c);
}

Line * MeshLoader::GenerateLine(vec4 &color)
{
	vector<vec3> v;
	v.push_back(vec3(0,0,0));
	v.push_back(vec3(0,0,0));

	vector<vec4> c;
	c.push_back(color);
	c.push_back(1.0f-color);

	return new Line(v, c);
}

Line * MeshLoader::GenerateTriangle()
{
	vector<vec3> v;
	v.push_back(vec3(-0.5f,-0.21f,0));
	v.push_back(vec3(0.5f,-0.21f,0));
	v.push_back(vec3(0.0f,0.42f,0));

	vector<vec4> c;
	c.push_back(vec4(1,0,1,1));
	c.push_back(vec4(1,0,1,1));
	c.push_back(vec4(1,0,1,1));

	return new Line(v, c);
}

Line * MeshLoader::GenerateTetrahedron()
{
	vector<vec3> v;
	v.push_back(vec3(-0.5f,-0.125f,-0.21f));
	v.push_back(vec3(0.5f,-0.125f,-0.21f));
	v.push_back(vec3(0.0f,-0.125f, 0.42f));
	v.push_back(vec3(-0.5f,-0.125f,-0.21f));
	v.push_back(vec3(0.0f, 0.375f, 0.0f));
	v.push_back(vec3(0.5f,-0.125f,-0.21f));
	v.push_back(vec3(0.0f, 0.375f, 0.0f));
	v.push_back(vec3(0.0f,-0.125f, 0.42f));

	vector<vec4> c;
	for(int i=0; i<v.size(); ++i)
		c.push_back(vec4(1,0,1,1));

	return new Line(v, c);
}

SkyboxMesh * MeshLoader::GenerateCubemapMesh(
	const string& PosXFilename, const string& NegXFilename,
	const string& PosYFilename, const string& NegYFilename,
	const string& PosZFilename, const string& NegZFilename)
{
	vec3 vertices[] = {
		vec3(-1.0f,  1.0f, -1.0f),
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f,  1.0f, -1.0f),
		vec3(-1.0f,  1.0f, -1.0f),

		vec3(-1.0f, -1.0f,  1.0f),
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(-1.0f,  1.0f, -1.0f),
		vec3(-1.0f,  1.0f, -1.0f),
		vec3(-1.0f,  1.0f,  1.0f),
		vec3(-1.0f, -1.0f,  1.0f),

		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f,  1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(1.0f,  1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),

		vec3(-1.0f, -1.0f,  1.0f),
		vec3(-1.0f,  1.0f,  1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(1.0f, -1.0f,  1.0f),
		vec3(-1.0f, -1.0f,  1.0f),

		vec3(-1.0f,  1.0f, -1.0f),
		vec3(1.0f,  1.0f, -1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(1.0f,  1.0f,  1.0f),
		vec3(-1.0f,  1.0f,  1.0f),
		vec3(-1.0f,  1.0f, -1.0f),

		vec3(-1.0f, -1.0f, -1.0f),
		vec3(-1.0f, -1.0f,  1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(-1.0f, -1.0f,  1.0f),
		vec3(1.0f, -1.0f,  1.0f)
	};

	std::vector<vec3> v(std::begin(vertices), std::end(vertices));

	SkyboxMesh * cubemap = new SkyboxMesh(v);
	cubemap->SetTexture(loadCubemapTexture(PosXFilename, NegXFilename, PosYFilename, NegYFilename, PosZFilename, NegZFilename));

	return cubemap;
}

GLuint MeshLoader::loadTexture(const std::string &path)
{
	GLuint id = SOIL_load_OGL_texture
		(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );

	return id;
}

void MeshLoader::normalizeVertices(std::vector<glm::vec3> &vertices)
{
	float maxX = FLT_MIN, minX = FLT_MAX;
	float maxY = FLT_MIN, minY = FLT_MAX;
	float maxZ = FLT_MIN, minZ = FLT_MAX;
	for(unsigned int i =0; i<vertices.size(); ++i)
	{
		if(vertices[i].x > maxX) maxX = vertices[i].x;
		else if(vertices[i].x < minX) minX = vertices[i].x;
		if(vertices[i].y > maxY) maxY = vertices[i].y;
		else if(vertices[i].y < minY) minY = vertices[i].y;
		if(vertices[i].z > maxZ) maxZ = vertices[i].z;
		else if(vertices[i].z < minZ) minZ = vertices[i].z;
	}

	float sizeX = maxX - minX;
	float sizeY = maxY - minY;
	float sizeZ = maxZ - minZ;
	for(unsigned int i =0; i<vertices.size(); ++i)
	{
		vertices[i] += vec3(sizeX / 2.0f - maxX, sizeY / 2.0f - maxY, sizeZ / 2.0f - maxZ);
		vertices[i] /= vec3(sizeX, sizeY, sizeZ);
	}
}

GLuint MeshLoader::loadCubemapTexture(
	const string& PosXFilename, const string& NegXFilename,
	const string& PosYFilename, const string& NegYFilename,
	const string& PosZFilename, const string& NegZFilename)
{
	string BaseDir = "..\\IET\\res\\skybox\\";

	string m_fileNames[6];
	m_fileNames[0] = BaseDir + PosXFilename;
	m_fileNames[1] = BaseDir + NegXFilename;
	m_fileNames[2] = BaseDir + PosYFilename;
	m_fileNames[3] = BaseDir + NegYFilename;
	m_fileNames[4] = BaseDir + PosZFilename;
	m_fileNames[5] = BaseDir + NegZFilename;

	GLuint textureID = SOIL_load_OGL_cubemap (m_fileNames[0].c_str(),m_fileNames[1].c_str(),m_fileNames[2].c_str(),m_fileNames[3].c_str(),m_fileNames[4].c_str(),m_fileNames[5].c_str(),
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_COMPRESS_TO_DXT
		);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (0 == textureID) {
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
		return -1;
	}

	return textureID;
}