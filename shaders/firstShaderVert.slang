import "vertexModule";

[shader("vertex")]

Vertex vertexMain(uint vertexID : SV_VertexID) : SV_Position {
    Vertex vertexes[3] = {
        {(0, 0.5, 0, 1), (1, 0, 0, 1)},
        {(-0.5, 0.5, 0, 1), (0, 1, 0, 1)},
        {(0.5, 0.5, 0, 1), (0, 0, 1, 1)}
    };
    return vertexes[vertexID];
}