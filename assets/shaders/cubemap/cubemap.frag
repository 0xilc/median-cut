#version 460 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    // Small epsilon value to avoid log(0)
    const float epsilon = 0.0001;
    
    // Fetch the texel from the skybox
    vec3 texel = texture(skybox, TexCoords).rgb;
    
    // Calculate the log color
    vec3 loggedColor = log(texel + vec3(epsilon));
    
    // Output the logged color
    FragColor = vec4(loggedColor, 1.0f);
}
