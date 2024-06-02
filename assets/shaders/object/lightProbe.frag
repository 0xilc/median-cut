#version 450 core

struct Light {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform samplerCube envSampler;

in vec3 fragWorldPos;
in vec3 fragWorldNor;
out vec4 fragColor;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 eyePos;
};

uniform int lightCount;
uniform Light lights[256];

void main(void)
{
    vec3 norm = normalize(fragWorldNor);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 fragPos = fragWorldPos.xyz;

    for (int i = 0; i < lightCount; i++)
    {
        vec3 lightDir = normalize(lights[i].direction);
        vec3 viewDir = normalize(eyePos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
        specular += spec * lights[i].color * 0.000006;

        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += diff * lights[i].color * 0.000006;
    }

    vec3 sum = specular + diffuse;
    vec3 logged = log(sum + vec3(0.00006));
    fragColor = vec4(logged, 1.0);
}
