#version 150

flat in  float Value;
     out vec4  FragColor;

void main()
{
    if (Value == 0.0)
        discard;
    FragColor = vec4(Value, 0, 0, 1);
}