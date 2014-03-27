#version 150

in  vec3 Position;
in  vec2 TexCoord;
out vec2 FragTexCoord;

void main()
{
    FragTexCoord = TexCoord;
    gl_Position  = vec4(Position, 1);
}