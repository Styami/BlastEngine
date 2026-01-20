module vertexModule;

public struct VSInput {
    public __init(float3 position, float3 color, float2 texCoord) {
        this.position = position;
        this.color = color;
        this.texCoord = texCoord;
    }
    public float3 position;
    public float3 color;
    public float2 texCoord;
};

public struct VSOutput {
    public __init(float4 position, float3 color, float2 texCoord) {
        this.position = position;
        this.color = color;
        this.texCoord = texCoord;

    }
    public float4 position : SV_Position;
    public float3 color;
    public float2 texCoord;
};