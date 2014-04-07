#version 150

in  vec3  Position;
in  vec2  TexCoord;
out float StrokeWidth;
out vec2  FragTexCoord;

uniform sampler2D Gradients;
uniform sampler2D StrokeWidths;

void main()
{
    vec2 pos = Position.xy / (textureSize(Gradients, 0) / 2) - vec2(1);
    vec2 tex = Position.xy / (textureSize(Gradients, 0));
    
    StrokeWidth = texture(StrokeWidths, tex).r;
    vec2 gradient = normalize( texture(Gradients, tex).xy );
    vec4 p1 = vec4(pos, 0, 1);
    vec4 p2 = vec4(pos + (gradient * StrokeWidth) / (textureSize(Gradients, 0) / 2), 0, 1);
    
    if (Position.z == 0.0)
        gl_Position = p1;
    else gl_Position = p2;
    
    FragTexCoord = TexCoord;
}