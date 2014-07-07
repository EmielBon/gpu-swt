#pragma include TextureUtil.fsh
#pragma include Codec.fsh

flat in  float ScatterID;
     out vec4  FragColor;

void main()
{
    FragColor = vec4(0, 0, 0, ScatterID);
}