struct VertexShaderOutput {
    float4 pos : SV_Position;
};
struct VertexShaderInput {
    float4 pos : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.pos = input.pos;
    return output;
}