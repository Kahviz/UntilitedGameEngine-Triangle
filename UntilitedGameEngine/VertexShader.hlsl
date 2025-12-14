struct VSOut
{
	float4 pos : SV_POSITION;
};

VSOut main(float2 pos : POSITION)
{
	VSOut o;
	o.pos = float4(pos, 0.0f, 1.0f);
	return o;
}
