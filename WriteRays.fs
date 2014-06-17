#version 150

flat in  float Value;
     out vec4  FragColor;

void main()
{
    FragColor = vec4(Value, 0, 0, 1);
}