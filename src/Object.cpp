#include "Object.h"

Object::Object()
{
}

void Object::SetMesh(std::shared_ptr<Mesh> mesh)
{
	m_Mesh = mesh;
}

void Object::SetShader(std::shared_ptr<Shader> shader)
{
	m_Shader = shader;
}

void Object::Update(float dt)
{
}

void Object::Render()
{
	m_Shader->Use();
	m_Shader->SetUniformMat4fv("model", glm::mat4(1.0f));
	m_Mesh->Draw();
}
