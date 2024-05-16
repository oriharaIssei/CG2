#include "Object3dTexture.hlsli"

struct WorldTransform {
    float4x4 world;
};
struct ViewProjection {
    float4x4 view;
    float4x4 projection;
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<ViewProjection> gViewProjection : register(b1);

struct VertexShaderInput {
    float4 pos : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    //mul は 行列の 積
    float4x4 vpvMat = mul(mul(gWorldTransform.world,gViewProjection.view),gViewProjection.projection);
    output.pos = mul(input.pos,vpvMat);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal,(float3x3)gWorldTransform.world));
    return output;
}