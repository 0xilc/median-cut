#version 420 core

uniform mat4 model;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 eyePos;
};

out vec3 fragWorldPos;
out vec3 fragWorldNor;

void main()
{
	vec4 temp = model * vec4(aPos, 1);
    fragWorldPos = temp.xyz / temp.w;
	fragWorldNor = inverse(transpose(mat3x3(model))) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}