#version 150

in  vec3 Position;
in  vec2 TexCoord;
out vec2 FragTexCoord;

void main()
{
    // Pass the tex coord straight through to the fragment shader
    FragTexCoord = TexCoord;
    gl_Position  = vec4(Position, 1);
}