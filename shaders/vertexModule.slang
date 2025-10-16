module vertexModule;

public struct Vertex {
    public __init(float4 position, float4 color) {
        this.position = position;
        this.color = color;
    }
    float4 position : SV_Position;
    float4 color : SV_Target0;
}