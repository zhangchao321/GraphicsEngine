cbuffer MatrixBuffer
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
};

struct VertexIn
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vin.pos.w = 1.0f;
	vout.pos = mul(vin.pos, gWorld);
	vout.pos = mul(vout.pos, gView);
	vout.pos = mul(vout.pos, gProj);

	vout.tex = vin.tex;

	return vout;
}