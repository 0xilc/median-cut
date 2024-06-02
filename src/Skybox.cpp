#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Skybox.h"
#include "GL/glew.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Skybox::Skybox()
{
    std::shared_ptr<Mesh> cube = std::make_shared<Mesh>("assets/models/cube.obj");
    m_SkyboxCube.SetMesh(cube);
    std::shared_ptr<Shader> cubemapShader = std::make_shared<Shader>("assets/shaders/cubemap/cubemap.vert", "assets/shaders/cubemap/cubemap.frag");
    m_SkyboxCube.SetShader(cubemapShader);
}

void Skybox::Load(const std::string& path)
{
    LoadHDR(path);
    ConstructCubemap();
}

void Skybox::Render()
{
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapID);
    m_SkyboxCube.Render();
    glDepthMask(GL_TRUE);
}

void Skybox::LoadHDR(const std::string& path)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(path.c_str(), &width, +&height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load HDR image." << std::endl;
    }
}

void Skybox::ConstructCubemap()
{
    int cubemapTexSize = 3000;

    glGenTextures(1, &m_CubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapID);
    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, cubemapTexSize, cubemapTexSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Render to 6 faces of the cube map texture via frame buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // Now render in 6 directions
    Mesh cube("assets/models/cube.obj");
    Shader equiToCube("assets/shaders/cubemap/equi_to_cube.vert", "assets/shaders/cubemap/equi_to_cube.frag");

    equiToCube.Use();
    equiToCube.SetUniform1i("equiMap", 0);
    equiToCube.SetUniformMat4fv("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glViewport(0, 0, cubemapTexSize, cubemapTexSize);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (int i = 0; i < 6; i++)
    {
        equiToCube.SetUniformMat4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubeMapID, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        cube.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
