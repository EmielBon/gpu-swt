#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;

     in  vec3  Position;
flat out float Value;

void main()
{
    float l      = fetch(Texture, Position.xy).r;
    float x      = int(l * 255);
    float binned = x / 255.0;
    Value        = 1; // Combined with additive blending, this records the frequency
    gl_Position  = vec4( getScreenSpaceCoord(Texture, vec2(x, 0)), 0, 1); // Does the binning (maybe the rounding is not even needed, but it won't hurt)
}