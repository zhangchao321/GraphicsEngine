cbuffer MatrixBuffer
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
}

cbuffer CameraBuffer
{
	float3 gCameraPos;
	float  padding;
}

struct VertexIn
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL0;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL0;
	float3 viewDir : NORMAL1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vin.pos.w = 1.0f;
	vout.pos = mul(vin.pos, gWorld);
	vout.pos = mul(vout.pos, gView);
	vout.pos = mul(vout.pos, gProj);

	vout.tex = vin.tex;
	vout.normal = mul(vin.normal, (float3x3)gWorld);
	vout.normal = normalize(vout.normal);

	float4 worldPos = mul(vin.pos, gWorld);
	vout.viewDir = gCameraPos.xyz - worldPos.xyz;
	vout.viewDir = normalize(vout.viewDir);

	return vout;
} 
