// This file was auto-generated by ".\Create-SphereShaders.ps1 -OutPath . -ResourceStart 1000" on 05/30/2022 11:45:33
#define QUAD_INST (1)
#define PER_VERTEX_INTENSITY (1)
#define RAYCASTING (1)
#define INSTANCING (1)
#define SphereRootSignature\
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
    "DescriptorTable(SRV(t0), SRV(t1), visibility = SHADER_VISIBILITY_VERTEX),"\
    "StaticSampler(s0, addressU = TEXTURE_ADDRESS_CLAMP, addressV = TEXTURE_ADDRESS_CLAMP, addressW = TEXTURE_ADDRESS_CLAMP, filter = FILTER_MIN_MAG_MIP_LINEAR, visibility = SHADER_VISIBILITY_VERTEX),"\
    "DescriptorTable(CBV(b0), CBV(b1), CBV(b2), visibility=SHADER_VISIBILITY_ALL)"
#include "SpherePixelShaderCore.hlsli"
