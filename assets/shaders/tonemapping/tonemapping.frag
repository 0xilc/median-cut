#version 460 core
out vec4 fragColor;

in vec2 fragTexCoords;

uniform sampler2D hdrTexture;
uniform float exposure;

const float delta = 0.0001; // Small epsilon value to avoid log(0)

void main() {
    vec3 loggedHdrColor = texture(hdrTexture, fragTexCoords).rgb;
    vec3 meanLoggedColor = textureLod(hdrTexture, vec2(0.5,0.5), 15).rgb;
    vec3 meanColor = exp(meanLoggedColor);
    vec3 hdrColor = exp(loggedHdrColor);
    
    float lum = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
    float meanLum = dot(meanColor, vec3(0.2126, 0.7152, 0.0722));
    float scaledLum = exposure * lum / (meanLum + delta);

    float lumTM = scaledLum / (1.0 + scaledLum);
    vec3 tmColor = clamp((lumTM / lum) * hdrColor, vec3(0.0), vec3(1.0));
    float p = 1.0 / 2.2;
    tmColor = pow(tmColor, vec3(p, p, p));

    fragColor = vec4(tmColor, 1.0);
}
