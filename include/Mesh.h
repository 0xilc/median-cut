#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};


struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};


class Mesh
{
public:
	Mesh();
	Mesh(const char* objPath);
	~Mesh();

	void LoadFromOBJ(const char* objPath);
	void SetupMesh();
	void Draw();


private:
	GLuint
		m_VAO,
		m_VertexAttribBuffer,
		m_IndexBuffer,
		m_EBO;

	std::vector<Texture> m_Textures;
	std::vector<Normal> m_Normals;
	std::vector<Vertex> m_Vertices;
	std::vector<Face> m_Faces;



	// Helpers
	void ProcessVertex(
		const std::string& vertexData, const std::vector<glm::vec3>& temp_positions,
		const std::vector<glm::vec2>& temp_texCoords, const std::vector<glm::vec3>& temp_normals);
};
