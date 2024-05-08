#include "Object3d.hlsli"

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 world;
};

cbuffer ConstantBuffer : register(b5) {
    TransformationMatrix gTransformationMatrix;
}

struct VertexShaderInput {
    float4 pos : POSITION0;
        float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    //mul は 行列の 積
    output.pos = mul(input.pos,gTransformationMatrix.WVP);
    output.normal = normalize(mul(input.normal,(float3x3)gTransformationMatrix.world));
    return output;
}