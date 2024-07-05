#include "Object3dTexture.hlsli"

struct Material {
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
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

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0); // SRVの registerは t
SamplerState gSampler : register(s0); // textureを読むためのもの. texture のサンプリングを担当

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;

    // texcoord を z=0 の (3+1)次元 として考える
    float4 transformedUV = mul(float4(input.texCoord,0.0f,1.0f),gMaterial.uvTransform);
    float4 textureColor = gMaterial.color * gTexture.Sample(gSampler,transformedUV.xy);
    if (textureColor.a <= 0.1f) {
        discard;
    }
    if (gMaterial.enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.enableLighting * textureColor.a;
    } else {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}