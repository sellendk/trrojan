// This file was auto-generated by ".\Create-SphereShaders.ps1 -OutPath . -ResourceStart 1000" on 06/08/2022 17:41:08
#define ADAPT_SPHERE_TESS (1)
#define SHADER_ROOT_SIGNATURE\
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
    "DescriptorTable(CBV(b0), CBV(b1), CBV(b2), visibility=SHADER_VISIBILITY_ALL)"
#include "SphereDomainShaderCore.hlsli"
