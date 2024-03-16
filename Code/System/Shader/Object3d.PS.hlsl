struct Material {
    float4 color;
};

///========================================
/// ConstantBufferの定義
///========================================
cbuffer ConstantBuffer : register(b0) {
    Material gMaterial;
}

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main() {
    PixelShaderOutput output;
    output.color = gMaterial.color;
    return output;
}