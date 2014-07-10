#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Components;

     in  vec3  Position;
flat out ivec2 CurrentXY;
flat out ivec2 CurrentRootXY;

void main()
{
    ivec2 current_xy      = ivec2(Position.xy);
    float current_root_id = fetch(Components, current_xy).a;
    ivec2 current_root_xy = decode(current_root_id);
    
    CurrentXY     = current_xy;
    CurrentRootXY = current_root_xy;
    
    vec2 pos = getScreenSpaceCoord(Components, current_root_xy);
    gl_Position = vec4(pos, 0, 1);
}