#include "Primitive.hlsli"

struct VertexShaderInput {
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct TransformationMatrix {
    float4x4 WVP;
};

cbuffer ConstantBuffer : register(b0) {
    TransformationMatrix gTransformationMatrix;
}

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    //mul は 行列の 積
    output.pos = mul(input.pos,gTransformationMatrix.WVP);
    output.color = input.color;
    return output;
}