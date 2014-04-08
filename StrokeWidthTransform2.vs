#version 150

in  vec3  Position;
in  vec2  TexCoord;
out float StrokeWidth;
out vec2  FragTexCoord;

uniform sampler2D Gradients;
uniform sampler2D StrokeWidths;

vec2 getScreenSpaceCoord(sampler2D sampler, vec2 screenTexCoord)
{
    return screenTexCoord / ((textureSize(sampler, 0) - vec2(1))) * 2 - vec2(1);
}

vec2 getTexCoord(sampler2D sampler, vec2 screenTexCoord)
{
    return screenTexCoord / (textureSize(Gradients, 0) - vec2(1));
}

void main()
{
    vec2 pos = getScreenSpaceCoord(Gradients, Position.xy);
    vec2 tex = getTexCoord(Gradients, Position.xy);
    
    StrokeWidth = texture(StrokeWidths, tex).r;
    vec2 gradient = normalize( texture(Gradients, tex).xy );
    vec4 p1 = vec4(pos, 0, 1);
    //vec4 p2 = vec4(getScreenSpaceCoord(Gradients, pos + gradient * StrokeWidth) + vec2(1), 0, 1);
    vec4 p2 = vec4(getScreenSpaceCoord(Gradients, Position.xy + gradient * StrokeWidth), 0, 1);
    //vec4 p2 = vec4(pos + (gradient * StrokeWidth) / ((textureSize(Gradients, 0) - vec2(1))) * 2, 0, 1);
    
    if (Position.z == 0.0)
        gl_Position = p1;
    else gl_Position = p2;
    
    FragTexCoord = TexCoord;
}