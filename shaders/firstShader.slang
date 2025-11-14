import "vertexModule";

[shader("vertex")]

Vertex vertexMain(uint vertexID: SV_VertexID) {
    Vertex vertexes[3] = {
        Vertex(float4(0.0, 0.5, 0.0, 1.0), float4(1.0, 0.0, 0.0, 1.0)),
        Vertex(float4(0.5, -0.5, 0, 1), float4(0, 1, 0, 1)),
        Vertex(float4(-0.5, -0.5, 0, 1), float4(0, 0, 1, 1))
    };
    return vertexes[vertexID];
}

[shader("fragment")]
float4 fragmentMain(float4 color: SV_Target0) : SV_Target0 {
    return color;
}