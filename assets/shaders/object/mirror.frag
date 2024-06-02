#version 420 core

uniform samplerCube envSampler;

in vec3 fragWorldPos;
in vec3 fragWorldNor;
out vec4 fragColor;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 eyePos;
};

void main(void)
{
    float epsilon = 0.0001;
    vec3 direction = fragWorldPos - eyePos;
    vec3 normal = fragWorldNor;
    vec3 refDir = reflect(direction, normal);
    
    vec3 color = texture(envSampler, refDir).rgb;
    vec3 loggedColor = log(color + epsilon);
    fragColor = vec4(loggedColor, 1.0);
}
