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
    ivec2 pos        = ivec2(Position.xy);
    // fetch not available on Mali GPU's T_T
    Value            = fetch(Values, pos).r;
    float lineLength = fetch(LineLengths, pos).r;
    vec2 gradient    = fetch(Gradients, pos).xy;
    gradient         = normalize(gradient) * (DarkOnLight ? 1 : -1);
    
    float z = Value / 50;
    vec4 p1 = vec4(getScreenSpaceCoord(Gradients, pos), z, 1);
    vec4 p2 = vec4(getScreenSpaceCoord(Gradients, pos + gradient * lineLength), z, 1);
    
    if (Position.z == 0.0)
        gl_Position = p1;
    else gl_Position = p2;
    
    FragTexCoord = TexCoord;
}