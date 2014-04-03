#version 150

in  vec2 FragTexCoord;
out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    vec4 color = texture(Texture, FragTexCoord);
    
    if (color.a == 0.0)
        discard;
    
    FragColor = color;
}