cbuffer ColorBuffer : register(b0)
{
    float4 cubeColor;
};

float4 main() : SV_Target
{
    return cubeColor;
}
