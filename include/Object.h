#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <memory>
class Object
{
public:
	Object();

	void SetMesh(std::shared_ptr<Mesh> mesh);
	void SetShader(std::shared_ptr<Shader> shader);

	void Update(float dt);
	void Render();

	std::shared_ptr<Shader> m_Shader;
private:
	std::shared_ptr<Mesh> m_Mesh;
};
