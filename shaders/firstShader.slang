import "vertexModule";

[shader("vertex")]

VSOutput vertexMain(VSInput input) {
  float4 position = float4(input.position, 0, 1);
  VSOutput output = VSOutput(position, input.color);
  return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return float4(input.color, 1);
}