#include "Object3d.hlsli"

struct Material {
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    float3 specularColor;
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

    output.color = gMaterial.color;
    
    int enableLighting = gMaterial.enableLighting;
    
    switch (gMaterial.enableLighting) {
        case 1:{
                float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
                float cos = pow(NdotL * 0.5f + 0.5f,2.0f);
                output.color.xyz = gMaterial.color.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
            };
        case 2:{
                float cos = saturate(dot(normalize(input.normal),-gDirectionalLight.direction));
                output.color.xyz = gMaterial.color.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
            };
        case 3:{
                float3 toEye = normalize(gViewProjection.cameraPos - input.worldPos);

                float cos = saturate(dot(normalize(input.normal),-gDirectionalLight.direction));

                float3 reflectLight = reflect(-gDirectionalLight.direction,normalize(input.normal));
                float rDotE = dot(reflectLight,toEye);
                float specularPow = pow(saturate(rDotE),gMaterial.shininess);

                float3 diffuse = gMaterial.color.rgb * gMaterial.color.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * gMaterial.specularColor;
                output.color.rgb = diffuse + specular;
            }
        case 4:{
                float3 toEye = normalize(gViewProjection.cameraPos - input.worldPos);

                float cos = saturate(dot(normalize(input.normal),-gDirectionalLight.direction));

                float3 halfVec = normalize(-gDirectionalLight.direction + toEye);
                float nDotH = dot(normalize(input.normal),halfVec);
                float specularPow = pow(saturate(nDotH),gMaterial.shininess);

                float3 diffuse = gMaterial.color.rgb * gMaterial.color.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * gMaterial.specularColor;
                output.color.rgb = diffuse + specular;
            }
    }
    
    return output;
}