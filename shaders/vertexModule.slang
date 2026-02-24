module vertexModule;

public struct VSInput {
    public __init(float3 position, float3 color, float3 normal, float2 texCoord) {
        this.position = position;
        this.color = color;
        this.texCoord = texCoord;
        this.normal = normal;
        this.indexMat = indexMat;
    }
    public float3 position;
    public float3 color;
    public float3 normal;
    public float2 texCoord;
    public int indexMat;
};

public struct VSOutput {
    public __init(float4 position, float3 color, float2 texCoord, int indexMat) {
        this.position = position;
        this.color = color;
        this.texCoord = texCoord;
        this.indexMat = indexMat;
    }
    public float4 position : SV_Position;
    public float3 color;
    public float2 texCoord;
    public int indexMat;
};

public struct MaterialObject {
    public float Ns;
    public float Ni;
    public float d;
    public int illum;
    public float4 Tf;
    public float4 Ka;
    public float4 Kd;
    public float4 Ks;
    public float4 Ke;
    public int indexAmbiantMap;
    public int indexDiffuseMap;
    public int indexSpecularMap;
    public int indexBumpMap;
};