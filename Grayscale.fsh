#version 150

uniform sampler2D Texture;

in  vec2 FragTexCoord;
out vec4 FinalColor;

void main()
{
    vec4 color = texture(Texture, FragTexCoord);
    float g = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    FinalColor = vec4(g, g, g, 1);
}