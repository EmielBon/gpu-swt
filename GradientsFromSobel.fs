#version 150

uniform sampler2D SobelHor;
uniform sampler2D SobelVer;

in  vec2 FragTexCoord;
out vec4 FragColor;

void main()
{
    vec2 gradient = vec2(
        texture(SobelHor, FragTexCoord).r,
        texture(SobelVer, FragTexCoord).r
    );
    
    gradient = normalize(gradient);
    
    FragColor = vec4(gradient, 0, 1);
}