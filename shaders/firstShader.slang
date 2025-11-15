import "vertexModule";

[shader("vertex")]

VSOutput vertexMain(VSInput input) {
  VSOutput output;
  output.position = float4(input.position, 0, 1);
  output.color = input.color;
  return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return float4(input.color, 1);
}