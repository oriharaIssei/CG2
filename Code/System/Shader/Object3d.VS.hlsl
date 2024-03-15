struct TransformationMatrix {
    float4x4 WVP;
};

cbuffer ConstantBuffer : register(b0) {
    TransformationMatrix gTransformationMatrix;
}

struct VertexShaderOutput {
    float4 pos : SV_Position;
};

struct VertexShaderInput {
    float4 pos : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    //mul は 行列の 積
    output.pos = mul(input.pos,gTransformationMatrix.WVP);
    return output;
}