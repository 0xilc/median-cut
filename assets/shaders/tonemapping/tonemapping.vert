#version 460 core
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

out vec2 fragTexCoords;

void main() {
    fragTexCoords = inTexCoords;
    gl_Position = vec4(inPosition, 0.0, 1.0);
}
