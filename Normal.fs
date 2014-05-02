#version 150

uniform sampler2D Texture;

in  vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(Texture, FragTexCoord);
}