#pragma once
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "MedianCut.h"
#include "Object.h"
#include "Skybox.h"
#include "SceneModel.h"

struct WindowState
{
	WindowState() :width(800), height(600), title("OpenGL"), fps(0) {}
	WindowState(int _width, int _height, std::string _title) :width(_width), height(_height), title(_title), fps(0) {}
	int width;
	int height;
	std::string title;
	float fps;
};

struct Tonemapping
{
	Tonemapping() = default;
	void Init();
	void BeginTonemapping();
	void EndTonemapping();

	void OnResize(int width, int height);
	GLuint FBO;
	GLuint ColorBuffer;
	GLuint DepthStencilBuffer;
	std::shared_ptr<Shader> tonemappingShader;
	GLuint quadVAO = 0;
	GLuint quadVBO;
	float exposure = 0.18f;
};

class App
{
public:
	App();
	~App();

	void Init();
	void Run();

	void Update(float deltaTime);
	void Render();

	void ProcessInput();
private:
	GLFWwindow* m_Window;
	WindowState m_WindowState;
	std::unique_ptr<Camera> m_Camera;
	std::shared_ptr<UBO> m_UBO;
	Tonemapping m_Tonemapping;
	std::unique_ptr<Skybox> m_Skybox;
	std::unique_ptr<SceneModel> m_Model;
	std::shared_ptr<MedianCut> m_MedianCut;
	int m_MedianCutLevel = 1;
};