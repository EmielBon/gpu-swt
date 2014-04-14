#version 150

uniform sampler2D Texture;
uniform float     Scale;

in  vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(Texture, FragTexCoord).rgb * Scale, 1);
}