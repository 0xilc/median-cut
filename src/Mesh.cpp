#include "Mesh.h"
#include "fstream"
#include "iostream"
#include "cassert"
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

Mesh::Mesh() {}

Mesh::Mesh(const char* objPath)
{
	LoadFromOBJ(objPath);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VertexAttribBuffer);
	glDeleteBuffers(1, &m_IndexBuffer);
	glDeleteBuffers(1, &m_EBO);
}

void Mesh::LoadFromOBJ(const char* objPath)
{
	std::fstream myfile;

	// Open the input 
	myfile.open(objPath, std::ios::in);

	if (myfile.is_open())
	{
		std::string curLine;

		while (getline(myfile, curLine))
		{
			std::stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			std::string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't') // texture
					{
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						m_Textures.push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						m_Normals.push_back(Normal(c1, c2, c3));
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						m_Vertices.push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

					m_Faces.push_back(Face(vIndex, tIndex, nIndex));
				}
				else
				{
					std::cout << "Ignoring unidentified line in obj file: " << curLine << std::endl;
				}
			}
		}

		myfile.close();
	}

	// Now SetupMesh
	SetupMesh();
}

void Mesh::SetupMesh()
{
	bool hasTexture = false;

	glGenVertexArrays(1, &m_VAO);
	assert(m_VAO > 0);
	glBindVertexArray(m_VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &m_VertexAttribBuffer);
	glGenBuffers(1, &m_IndexBuffer);

	assert(m_VertexAttribBuffer > 0 && m_IndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

	int vertexDataSizeInBytes = m_Vertices.size() * 3 * sizeof(GLfloat);
	int normalDataSizeInBytes = m_Normals.size() * 3 * sizeof(GLfloat);
	int textureDataSizeInBytes = m_Textures.size() * 2 * sizeof(GLfloat);
	int indexDataSizeInBytes = m_Faces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[m_Vertices.size() * 3];
	GLfloat* normalData = new GLfloat[m_Normals.size() * 3];
	GLfloat* textureData = new GLfloat[m_Textures.size() * 2];
	GLuint* indexData = new GLuint[m_Faces.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i < m_Vertices.size(); ++i)
	{
		vertexData[3 * i] = m_Vertices[i].x;
		vertexData[3 * i + 1] = m_Vertices[i].y;
		vertexData[3 * i + 2] = m_Vertices[i].z;

		minX = std::min(minX, m_Vertices[i].x);
		maxX = std::max(maxX, m_Vertices[i].x);
		minY = std::min(minY, m_Vertices[i].y);
		maxY = std::max(maxY, m_Vertices[i].y);
		minZ = std::min(minZ, m_Vertices[i].z);
		maxZ = std::max(maxZ, m_Vertices[i].z);
	}

	for (int i = 0; i < m_Normals.size(); ++i)
	{
		normalData[3 * i] = m_Normals[i].x;
		normalData[3 * i + 1] = m_Normals[i].y;
		normalData[3 * i + 2] = m_Normals[i].z;
	}

	if (!m_Textures.empty()) hasTexture = true;
	for (int i = 0; i < m_Textures.size(); ++i)
	{
		textureData[2 * i] = m_Textures[i].u;
		textureData[2 * i + 1] = m_Textures[i].v;

	}

	for (int i = 0; i < m_Faces.size(); ++i)
	{
		indexData[3 * i] = m_Faces[i].vIndex[0];
		indexData[3 * i + 1] = m_Faces[i].vIndex[1];
		indexData[3 * i + 2] = m_Faces[i].vIndex[2];
	}
	if (hasTexture)
	{
		glBufferData(GL_ARRAY_BUFFER, vertexDataSizeInBytes + normalDataSizeInBytes + textureDataSizeInBytes, 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSizeInBytes, vertexData);
		glBufferSubData(GL_ARRAY_BUFFER, vertexDataSizeInBytes, normalDataSizeInBytes, normalData);
		glBufferSubData(GL_ARRAY_BUFFER, vertexDataSizeInBytes + normalDataSizeInBytes, textureDataSizeInBytes, textureData);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

		// done copying to GPU memory; can free now from CPU memory
		delete[] vertexData;
		delete[] normalData;
		delete[] textureData;
		delete[] indexData;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSizeInBytes));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSizeInBytes + normalDataSizeInBytes));
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, vertexDataSizeInBytes + normalDataSizeInBytes, 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSizeInBytes, vertexData);
		glBufferSubData(GL_ARRAY_BUFFER, vertexDataSizeInBytes, normalDataSizeInBytes, normalData);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

		// done copying to GPU memory; can free now from CPU memory
		delete[] vertexData;
		delete[] normalData;
		delete[] indexData;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSizeInBytes));
	}
}

void Mesh::Draw()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Faces.size() * 3, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}