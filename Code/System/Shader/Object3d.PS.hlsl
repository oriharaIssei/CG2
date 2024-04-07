#include "Object3d.hlsli"

struct Material {
    float4 color;
    int enableLighting;
};

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

struct DirectionalLight {
    float4 color;
    float3 direction;
    float intensity;
};
///========================================
/// ConstantBufferの定義
///========================================
cbuffer ConstantBuffer : register(b0) {
    Material gMaterial;
}
cbuffer ConstantBuffer : register(b1) {
    DirectionalLight gDirectionalLight;
}
Texture2D<float4> gTexture : register(t0); // SRVの registerは t
SamplerState gSampler : register(s0); // textureを読むためのもの. texture のサンプリングを担当

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float4 textureColor = gMaterial.color * gTexture.Sample(gSampler,input.texCoord);
    if(gMaterial.enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    } else {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}