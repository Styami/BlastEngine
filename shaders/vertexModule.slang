module vertexModule;

public struct VSInput {
    public __init(float3 position, float3 color) {
        this.position = position;
        this.color = color;
    }
    public float3 position;
    public float3 color;
};

public struct VSOutput {
    public __init(float4 position, float3 color) {
        this.position = position;
        this.color = color;
    }
    public float4 position : SV_Position;
    public float3 color;
};