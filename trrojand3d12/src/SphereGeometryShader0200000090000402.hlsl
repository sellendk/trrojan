// This file was auto-generated by ".\Create-SphereShaders.ps1 -OutPath . -ResourceStart 1000" on 06/17/2022 15:37:02
#define GEO_QUAD (1)
#define PER_VERTEX_RADIUS (1)
#define RAYCASTING (1)
#define SHADER_ROOT_SIGNATURE\
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
    "DescriptorTable(CBV(b0), CBV(b1), CBV(b2), visibility=SHADER_VISIBILITY_ALL)"
#include "SphereGeometryShaderCore.hlsli"
