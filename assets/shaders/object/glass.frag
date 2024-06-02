#version 420 core

uniform samplerCube envSampler;

in vec4 fragWorldPos;
in vec3 fragWorldNor;
out vec4 fragColor;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 eyePos;
};

const float refractiveIndex = 1.0 / 1.52;

void main(void)
{
    vec3 incidentDir = normalize(fragWorldPos.xyz - eyePos);
    vec3 normal = normalize(fragWorldNor);
    vec3 refractedDir = refract(incidentDir,normal, refractiveIndex);
    vec3 lookupDir = normalize(refractedDir);
    vec3 color = texture(envSampler, lookupDir).rgb;
    vec3 loggedColor = log(color + vec3(0.00006));
    fragColor = vec4(loggedColor, 1.0);
}
