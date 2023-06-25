
cbuffer CB : register(b0)
{
    float4x4 g_Transform;
};

void main_vs(
  float2 i_pos : POSITION,
  float2 i_uv : UV,
  float4 i_color : COLOR,
  out float2 o_uv : UV,
  out float4 o_color : COLOR,
  out float4 o_pos : SV_Position
)
{
  o_uv = i_uv;
  o_color = i_color;
  o_pos = mul(float4(i_pos, 0, 1), g_Transform);
}

Texture2D t_Texture : register(t0);
SamplerState s_Sampler : register(s0);

void main_ps(
  float2 i_uv : UV,
  float4 i_color : COLOR,
	out float4 o_color : SV_Target0
)
{
  o_color = i_color * t_Texture.Sample(s_Sampler, i_uv);
}
