#version 150

uniform vec3 Color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(Color, 1);
}