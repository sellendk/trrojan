/// <copyright file="RayPassVertexShader.hlsl" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "TwoPassVolumePipeline.hlsli"


/// <summary>
/// Vertex shader for the ray computation pass in two-pass volume rendering.
/// </summary>
/// <remarks>
/// The shader assumes the geometry being a unit-sized cube around the origin of
/// the coordinate system.
/// </remarks>
/// <param name="input"></param>
/// <returns></returns>
VsOutput Main(VsInput input) {
    VsOutput retval = (VsOutput) 0;

    retval.Ray = float4(input.Position, 1.0f);
    retval.Position = mul(retval.Ray, ViewProjMatrix);

    return retval;
}
