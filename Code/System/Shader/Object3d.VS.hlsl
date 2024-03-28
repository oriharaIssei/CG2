#include "Object3d.hlsli"

struct TransformationMatrix {
    float4x4 WVP;
};

cbuffer ConstantBuffer : register(b0) {
    TransformationMatrix gTransformationMatrix;
}

struct VertexShaderInput {
    float4 pos : POSITION0;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    //mul は 行列の 積
    output.pos = mul(input.pos,gTransformationMatrix.WVP);
    output.texCoord = input.texCoord;
    return output;
}