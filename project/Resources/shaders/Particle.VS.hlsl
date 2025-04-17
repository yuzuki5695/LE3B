#include "Particle.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};

struct ParticleForGRU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};

//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
StructuredBuffer<ParticleForGRU> gParticle : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.World));
    output.color = gParticle[instanceId].color;
    return output;
}