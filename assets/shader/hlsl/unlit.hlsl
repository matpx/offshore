#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
};

void main_vs(
	float3 i_pos : POSITION,
	float3 i_normal : NORMAL,
	out float3 o_normal : NORMAL,
	out float4 o_pos : SV_Position
)
{
  o_normal = i_normal;
  o_pos = mul(float4(i_pos, 1), g_Transform); // TODO order?
}

void main_ps(
	float3 i_normal : NORMAL,
	out float4 o_color : SV_Target0
)
{
	i_normal.x = 0.5f;
  o_color = float4(i_normal, 1);
}
