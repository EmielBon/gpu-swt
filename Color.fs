#version 150

flat in vec4 Color;
in vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(1, 1, 1, 1);//Color;
}