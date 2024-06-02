#include "Camera.h"
#include "string"
#include "Input.h"
#include <functional>
#include <iostream>

Camera::Camera(int screenWidth, int screenHeight, float fov)
{
    // Set up properties
    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;
    m_FOV = fov;
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;
    m_Position = glm::vec3(0.0f, 0.0f, 3.0f);
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Yaw = -90.0f;
    m_Pitch = 0.0f;
    m_MovementSpeed = 10.0f;
    m_MouseSensitivity = 1.0f;
    m_Zoom = 45.0f;
    m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_DistanceFromCamera = 4.0f;

    m_CallbackId = Input::SubscribeMouseMoved(MouseCallback, this);
    UpdateCameraVectors();
    UpdateMatrices();
}

Camera::~Camera()
{
    if (m_CallbackId != -1) {
        Input::UnsubscribeMouseMoved(m_CallbackId);
    }
}

void Camera::Update(float dt)
{
    ProcessKeyboard(dt);
}

void Camera::SetUniforms(UBO& ubo)
{
    ubo.UpdateUBO("CameraData", 0, sizeof(glm::mat4), glm::value_ptr(m_ViewMatrix));
    ubo.UpdateUBO("CameraData", sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_ProjectionMatrix));
    ubo.UpdateUBO("CameraData", 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(m_Position));
}


void Camera::OnResize(int screenWidth, int screenHeight)
{
    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;
    UpdateMatrices();
}

void Camera::UpdateMatrices()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Target , m_Up);
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_Near, m_Far);
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    if(Input::IsKeyPressed(GLFW_MOUSE_BUTTON_RIGHT) == 0)
		return;

    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw -= xoffset;
    m_Pitch -= yoffset;

    m_Pitch = std::max(std::min(m_Pitch, 89.0f), -89.0f); 

    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;
    if (glfwGetKey(Input::s_Window, GLFW_KEY_Z) == GLFW_PRESS)
        m_DistanceFromCamera -= velocity;
    if (glfwGetKey(Input::s_Window, GLFW_KEY_X) == GLFW_PRESS)
        m_DistanceFromCamera += velocity;
    if (glfwGetKey(Input::s_Window, GLFW_KEY_A) == GLFW_PRESS)
		m_Yaw -= velocity * 8;
    if (glfwGetKey(Input::s_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Yaw += velocity * 8;
	
    UpdateCameraVectors();
	UpdateMatrices();
}


void Camera::MouseCallback(void* self, float x, float y)
{
    static_cast<Camera*>(self)->ProcessMouseMovement(x, y);
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);

    m_Position = m_Target - m_Front * m_DistanceFromCamera;

    auto m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp)); 
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

