/// <copyright file="ColourConversion.hlsli" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>


/// <summary>
/// Converts an 8-bit RGBA colour to <c>float4</c>.
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
/// <returns></returns>
float4 UintToFloat4Colour(uint colour) {
    float4 retval;

    retval.r = (colour & 0xFF) / 255.0f;
    colour >>= 8;
    retval.g = (colour & 0xFF) / 255.0f;
    colour >>= 8;
    retval.b = (colour & 0xFF) / 255.0f;
    colour >>= 8;
    retval.a = colour / 255.0f;

    return retval;
}
