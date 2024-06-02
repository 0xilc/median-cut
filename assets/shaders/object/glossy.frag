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
    vec3 fragNormal = normalize(fragWorldNor);
    vec3 fragPosition = fragWorldPos.xyz;

    vec3 specular = vec3(0.0);
    vec3 diffuse = vec3(0.0);


    vec3 viewDir = normalize(eyePos - fragPosition);
    vec3 refDir = reflect(-viewDir, fragWorldNor);
    vec3 mirrorColor = texture(envSampler, refDir).rgb;

    for (int i = 0; i < lightCount; i++)
    {
        vec3 lightColor = normalize(lights[i].color) * lights[i].intensity;
        vec3 lightDir = normalize(lights[i].direction);
        
        vec3 reflectDir = reflect(-lightDir, fragNormal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
        specular += 1 * spec * lightColor;

        float diff = max(dot(fragNormal, lightDir), 0.0);
        diffuse += lightColor * diff;
    }

    vec3 sum = (specular + diffuse) * 0.8 + mirrorColor * 0.2;
    vec3 logged = log(sum + vec3(0.00006));
    fragColor = vec4(logged, 1.0);
}
