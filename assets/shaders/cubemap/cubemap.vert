#version 420 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
};

void main()
{
    mat4 centeredView = mat4(mat3(view));
    TexCoords = aPos;
    gl_Position = projection * centeredView * vec4(aPos, 1.0);
}  