#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Gradients;
uniform sampler2D LineLengths;
uniform sampler2D Values;
uniform bool      DarkOnLight;

     in  vec3  Position;
     in  vec2  TexCoord;
     out vec2  FragTexCoord;
flat out float Value;

void main()
{
    Value = fetch(Values, Position.xy).r;
    float lineLength = fetch(LineLengths, Position.xy).r;
    vec2 gradient = normalize( fetch(Gradients, Position.xy).xy) * (DarkOnLight ? 1 : -1);
    float z = Value / 50;
    vec4 p1 = vec4(getScreenSpaceCoord(Gradients, Position.xy), z, 1);
    vec4 p2 = vec4(getScreenSpaceCoord(Gradients, Position.xy + gradient * lineLength), z, 1);
    
    if (Position.z == 0.0)
        gl_Position = p1;
    else gl_Position = p2;
    
    FragTexCoord = TexCoord;
}