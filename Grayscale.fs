#version 150

uniform sampler2D Texture;

in  vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    vec3 color = texture(Texture, FragTexCoord).rgb;
    vec3 g = vec3( dot(color, vec3(0.2126, 0.7152, 0.0722)) );
    FragColor = vec4(g, 1);
}