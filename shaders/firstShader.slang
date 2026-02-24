import "vertexModule";

ConstantBuffer<float4x4> mvp;

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
  float4x4 tmp = transpose(mvp);
  float4 position = mul(tmp, float4(input.position, 1));
  VSOutput output = VSOutput(position, input.color, input.texCoord, input.indexMat);
  return output;
}

Sampler2D[] textures;
StructuredBuffer<MaterialObject> materials;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  float4 color;
  if(input.indexMat < 0)
    color = float4(input.color, 1);
  else
    color = textures[materials[input.indexMat].indexDiffuseMap].Sample(input.texCoord);
  return color;
}