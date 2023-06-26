#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
};

void main_vs(
	float3 i_pos : POSITION,
	out float4 o_pos : SV_Position
)
{
  o_pos = mul(float4(i_pos, 1), g_Transform); // TODO order?
}

void main_ps(
	out float4 o_color : SV_Target0
)
{
  o_color = float4(1, 0, 0, 1);
}
