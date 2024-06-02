#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "MedianCut.h"
#include <vector>

class SceneModel
{
public:
	SceneModel();

	void Init();
	void Render(std::shared_ptr<MedianCut> mc);

	void SetCurrentShaderIndex(int shaderIndex);

	int m_CurrentShaderIndex = 0;
	std::vector<std::shared_ptr<Shader>> m_Shaders;
private:
	std::shared_ptr<Mesh> m_Mesh;
};