#version 420 core

out vec4 FragColor;

in vec3 localPos;

uniform sampler2D equiMap;

void main()
{   
    // Sample from Equilateral Image
    float PI = 3.14159265358979323846;
    float phi = atan(localPos.z, localPos.x);      
    float theta = atan(localPos.y, length(localPos.xz)); 
    
    float u = (phi + PI) / (2.0 * PI); 
    float v = (theta + PI * 0.5) / PI; 
    vec4 texColor = texture(equiMap, vec2(u, v));

    FragColor = texColor;
}
