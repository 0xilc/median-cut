#include "App.h"
#include "MedianCut.h"
#include "Object.h"
#include "Input.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Skybox.h"

App::App()
{
    m_WindowState = WindowState(2000, 1500, "Median Cut Sampling");
    Init();
}

App::~App()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void App::Init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (!glfwInit())
    {
        exit(-1);
    }

    m_Window = glfwCreateWindow(m_WindowState.width, m_WindowState.height, m_WindowState.title.c_str(), NULL, NULL);

    if (!m_Window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(m_Window);
    // glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }

    glfwSetWindowUserPointer(m_Window, this);

    // glfw state
    glfwSwapInterval(0);
    glEnable(GL_DEPTH_TEST);

    // GLFW Events
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
            App* app = (App*)glfwGetWindowUserPointer(window);
            app->m_WindowState.width = width;
            app->m_WindowState.height = height;
            app->m_Camera->OnResize(width, height);
            app->m_Tonemapping.OnResize(width, height);
        });

    // Key callbacks
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        App* app = (App*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_1:
                app->m_Model->SetCurrentShaderIndex(0);
                break;
            case GLFW_KEY_2:
                app->m_Model->SetCurrentShaderIndex(1);
                break;
            case GLFW_KEY_3:
                app->m_Model->SetCurrentShaderIndex(2);
                break;
            case GLFW_KEY_4:
                app->m_Model->SetCurrentShaderIndex(3);
                break;
            case GLFW_KEY_5:
                app->m_Model->SetCurrentShaderIndex(4);
                break;
            case GLFW_KEY_W:
                app->m_Tonemapping.exposure *= 2.0f;
                break;
            case GLFW_KEY_S:
                app->m_Tonemapping.exposure /= 2.0f;
                break;
            case GLFW_KEY_E:
                if (app->m_MedianCutLevel == 0) return;
                app->m_MedianCutLevel--;
                app->m_MedianCut->Process(app->m_MedianCutLevel);
                std::cout << "Num lights: " << app->m_MedianCut->GetNumLights() << std::endl;
                break;
            case GLFW_KEY_R:
                if (app->m_MedianCutLevel == 7) return;
                app->m_MedianCutLevel++;
                app->m_MedianCut->Process(app->m_MedianCutLevel);
                std::cout << "Num lights: " << app->m_MedianCut->GetNumLights() << std::endl;
                break;
            default:
                break;
            }
        }
    });

    // Input setup
    Input::Init(m_Window);

    // Camera setup
    m_Camera = std::make_unique<Camera>(this->m_WindowState.width, m_WindowState.height, 90.0f);

    // UBO setup
    m_UBO = std::make_unique<UBO>();

    // Skybox setup
    m_Skybox = std::make_unique<Skybox>();

    // Object setup
    m_Model = std::make_unique<SceneModel>();
    m_Model->Init();

    // Median Cut setup
    m_MedianCut = std::make_shared<MedianCut>();

    // Tonemapping setup
    m_Tonemapping.Init();

}

void App::Run()
{
    std::string mapPath = "assets/maps/Thumersbach.hdr";
    m_UBO->CreateUBO("CameraData", UBOBindingPoints::CAMERA_DATA, 2 * sizeof(glm::mat4) + sizeof(glm::vec3));

    double currentFrame = glfwGetTime();
    double lastFrame = currentFrame;
    double deltaTime;

    m_Skybox->Load(mapPath);

    m_MedianCut->LoadHDRI(mapPath);
    m_MedianCut->Process(m_MedianCutLevel);

    while (!glfwWindowShouldClose(m_Window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        m_WindowState.fps = 1.0f / deltaTime;

        Update(deltaTime);
        m_Tonemapping.BeginTonemapping();
        Render();
        m_Tonemapping.EndTonemapping();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void App::Update(float deltaTime)
{
    ProcessInput();
    m_Camera->Update(deltaTime);
    m_Camera->SetUniforms(*m_UBO);
}

void App::Render()
{
    glViewport(0, 0, m_WindowState.width, m_WindowState.height);
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_Skybox->Render();
    m_Model->Render(m_MedianCut);
}

void App::ProcessInput()
{
}

void Tonemapping::Init()
{
    // init shader
    tonemappingShader = std::make_shared<Shader>("assets/shaders/tonemapping/tonemapping.vert", "assets/shaders/tonemapping/tonemapping.frag");

    // init quad
    if (quadVAO == 0) {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    }

    OnResize(2000, 1500);
}

void Tonemapping::BeginTonemapping()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tonemapping::EndTonemapping()
{
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Generate mipmaps for the color attachment
    glBindTexture(GL_TEXTURE_2D, ColorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Synchronize texture access
    glTextureBarrier();

    glBindTexture(GL_TEXTURE_2D, ColorBuffer);
    // Render quad
    tonemappingShader->Use();
    tonemappingShader->SetUniform1f("exposure", exposure);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Tonemapping::OnResize(int width, int height)
{
    // Initialize framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Color buffer
    glGenTextures(1, &ColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

    // Depth-stencil buffer
    glGenRenderbuffers(1, &DepthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
