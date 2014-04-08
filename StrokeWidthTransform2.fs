#version 150

in float StrokeWidth;
out vec4 FragColor;

void main()
{
    FragColor = vec4( vec3(StrokeWidth), 1);
    if (StrokeWidth == 0.0)
        FragColor = vec4(1);
}