#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Object.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox();

	void Load(const std::string& path);
	void Render();

private:
	void LoadHDR(const std::string& path);
	void ConstructCubemap();
	Object m_SkyboxCube;
	Shader m_Shader;
	uint32_t m_TextureID;
	uint32_t m_CubeMapID;

};