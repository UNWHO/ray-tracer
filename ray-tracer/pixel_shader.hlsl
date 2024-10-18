Texture2D tex;

float4 main(float4 pos: SV_Position) : SV_TARGET
{
    return tex.Load(int3(pos.xy, 0));
}