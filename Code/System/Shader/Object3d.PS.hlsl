#include "Object3d.hlsli"

struct Material {
    float4 color;
};

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

///========================================
/// ConstantBufferの定義
///========================================
cbuffer ConstantBuffer : register(b0) {
    Material gMaterial;
}

Texture2D<float4> gTexture : register(t0); // SRVの registerは t
SamplerState gSampler : register(s0); // textureを読むためのもの. texture のサンプリングを担当

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    // 入力された色とテクスチャの色を反映させるために乗算させる
    output.color = gMaterial.color * gTexture.Sample(gSampler,input.texCoord);
    return output;
}