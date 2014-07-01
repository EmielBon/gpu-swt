#version 150

flat in  vec4 BoundingBox;
     out vec4 FragColor;

void main()
{
    FragColor = BoundingBox;
}