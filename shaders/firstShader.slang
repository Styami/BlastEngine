import "vertexModule";

[shader("vertex")]

VSOutput vertexMain(uint vertexID : SV_VertexID) {
    VSOutput vertexes[3] = {
        VSOutput(float4(0.0, 0.5, 0.0, 1.0), float3(1.0, 0.0, 0.0)),
        VSOutput(float4(0.5, -0.5, 0, 1), float3(0, 1, 0)),
        VSOutput(float4(-0.5, -0.5, 0, 1), float3(0, 0, 1))
    };
  return vertexes[vertexID];
  //return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return float4(input.color, 1);
}