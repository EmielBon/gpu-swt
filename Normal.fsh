#version 150

uniform sampler2D Texture;
uniform int Channels = 3;

in  vec2 FragTexCoord;
out vec4 FinalColor;

void main()
{
    vec4 color = texture(Texture, FragTexCoord);
    
    if (Channels == 1)
        color.gb = color.rr;
    
    FinalColor = color;
}