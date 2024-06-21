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

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    if(gMaterial.enableLighting != 0) {
        float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
        output.color = gMaterial.color * gMaterial.color * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    } else {
        output.color = gMaterial.color * gMaterial.color;
    }
    return output;
}