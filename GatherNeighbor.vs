#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include SWTUtil.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform sampler2D StrokeWidths;
uniform int       Column;

     in  vec3  Position;
flat out float ScatterID;

void main()
{
    ivec2 current_xy      = ivec2(Position.xy) + ivec2(Column, 0);
    float current_root_id = fetch(Texture, current_xy).a;
    ivec2 current_root_xy = decode(current_root_id);
    
    ivec2 neighbor_xy      = current_xy + ivec2(1, 0);
    float neighbor_root_id = fetch(Texture, neighbor_xy).a;
    
    float current_sw  = fetch(StrokeWidths, current_xy).r;
    float neighbor_sw = fetch(StrokeWidths, neighbor_xy).r;
    
    ScatterID = ifelse(getRatio(current_sw, neighbor_sw) <= 3.0, neighbor_root_id, 0.0);
    
    gl_Position = vec4(getScreenSpaceCoord(Texture, current_root_xy), 0, 1);
}