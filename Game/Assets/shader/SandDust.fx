struct SPSIn
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent  : TANGENT;
	float3 biNormal : BINORMAL;
	float2 uv : TEXCOORD0;
};

cbuffer DustCb : register(b1)
{
	float4 dustColor;
};

#include "ModelVSCommon.h"

Texture2D<float4> g_albedo : register(t0);
sampler g_sampler : register(s0);

SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal, uniform bool isUsePreComputedVertexBuffer)
{
	SPSIn psIn;
	psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal, isUsePreComputedVertexBuffer);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);
	CalcVertexNormalTangentBiNormalInWorldSpace(
		psIn.normal,
		psIn.tangent,
		psIn.biNormal,
		mWorldLocal,
		vsIn.normal,
		vsIn.tangent,
		vsIn.biNormal,
		isUsePreComputedVertexBuffer
	);
	psIn.uv = vsIn.uv;
	return psIn;
}

float4 PSMain(SPSIn psIn) : SV_Target0
{
	float4 albedo = g_albedo.Sample(g_sampler, psIn.uv);
	float alpha = albedo.a * dustColor.a;
	clip(alpha - 0.004f);
	return float4(albedo.rgb * dustColor.rgb, alpha);
}
