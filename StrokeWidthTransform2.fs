#version 150

in float StrokeWidth;
out vec4 FragColor;

void main()
{
    FragColor = vec4( vec3(StrokeWidth / 50), 1);
    if (StrokeWidth == 0.0)
        discard;//FragColor = vec4(1);
}