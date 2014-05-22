#version 150

uniform float Value;

in  vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(Value, 1);
}