#include "SceneModel.h"

SceneModel::SceneModel()
{
}

void SceneModel::Init()
{
	m_Mesh = std::make_unique<Mesh>("assets/models/sphere.obj");
	m_Shaders.push_back(std::make_shared<Shader>("assets/shaders/object/lightProbe.vert", "assets/shaders/object/lightProbe.frag"));
	m_Shaders.push_back(std::make_shared<Shader>("assets/shaders/object/mirror.vert", "assets/shaders/object/mirror.frag"));
	m_Shaders.push_back(std::make_shared<Shader>("assets/shaders/object/glass.vert", "assets/shaders/object/glass.frag"));
	m_Shaders.push_back(std::make_shared<Shader>("assets/shaders/object/glossy.vert", "assets/shaders/object/glossy.frag"));
	m_Shaders.push_back(std::make_shared<Shader>("assets/shaders/object/disco.vert", "assets/shaders/object/disco.frag"));
}

void SceneModel::Render(std::shared_ptr<MedianCut> mc)
{
	std::shared_ptr<Shader> currentShader = m_Shaders[m_CurrentShaderIndex];
	currentShader->Use();
	currentShader->SetUniformMat4fv("model", glm::mat4(1.0f));
	currentShader->SetUniform1i("lightCount", mc->GetNumLights());

	// Set lights as uniform array
	GLuint shaderProgram = currentShader->m_Program;
	GLint lightsLoc = glGetUniformLocation(shaderProgram, "lights");
	int lightCount = mc->GetNumLights();
	std::vector<Light> lights = mc->GetLights();
	for (int i = 0; i < lightCount; ++i) {
		std::string index = std::to_string(i);
		glUniform3fv(glGetUniformLocation(shaderProgram, ("lights[" + index + "].direction").c_str()), 1, glm::value_ptr(lights[i].direction));
		glUniform3fv(glGetUniformLocation(shaderProgram, ("lights[" + index + "].color").c_str()), 1, glm::value_ptr(lights[i].color));
		glUniform1f(glGetUniformLocation(shaderProgram, ("lights[" + index + "].intensity").c_str()), lights[i].intensity);
	}

	m_Mesh->Draw();
}

void SceneModel::SetCurrentShaderIndex(int index)
{
	if (index < 0 || index > 4) return;
	m_CurrentShaderIndex = index;
}
