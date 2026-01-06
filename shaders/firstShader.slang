import "vertexModule";

ConstantBuffer<float4x4> mvp;

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
  float4x4 tmp = transpose(mvp);
  float4 position = mul(tmp, float4(input.position, 1));
  VSOutput output = VSOutput(position, input.color);
  return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return float4(input.color, 1);
}