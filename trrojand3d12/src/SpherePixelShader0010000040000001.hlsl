// This file was auto-generated by ".\Create-SphereShaders.ps1 -OutPath . -ResourceStart 1000" on 05/30/2022 17:59:54
#define ADAPT_HEMISPHERE_TESS (1)
#define PER_VERTEX_COLOUR (1)
#define SphereRootSignature\
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
    "DescriptorTable(CBV(b0), CBV(b1), CBV(b2), visibility=SHADER_VISIBILITY_ALL)"
#include "SpherePixelShaderCore.hlsli"
