#version 150

in  vec2 FragTexCoord;
out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    vec4 color = texture(Texture, FragTexCoord);
    
    if (color.r < 0.5)
        discard;
    
    FragColor = color;
}