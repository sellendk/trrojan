[CmdletBinding()]
param([Parameter(Mandatory = $true, ValueFromPipeline = $true)] [string] $OutPath,
    [string] $IncludeFile = "sphere_techniques.h",
    [string] $ResourceFile = "sphere_techniques.rc",
    [uint16] $ResourceStart = 256,
    [string] $ResourceType = "SHADER")

begin {
    # Properties of the rendering techniques.
    $SPHERE_TECHNIQUE_USE_GEO = ([uint64] 1) -shl 31
    $SPHERE_TECHNIQUE_USE_TESS = ([uint64] 1) -shl 30
    $SPHERE_TECHNIQUE_USE_SRV =  ([uint64] 1) -shl 29
    $SPHERE_TECHNIQUE_USE_RAYCASTING = ([uint64] 1) -shl 28
    $SPHERE_TECHNIQUE_USE_INSTANCING = ([uint64] 1) -shl 27
    
    # Rendering techniques.    
    $__SPHERE_TECH_BASE = ([uint64] 1) -shl 63
    $global:techniques = @{}
    $global:techniques[($__SPHERE_TECH_BASE -shr 0) -bor $SPHERE_TECHNIQUE_USE_SRV -bor $SPHERE_TECHNIQUE_USE_RAYCASTING -bor $SPHERE_TECHNIQUE_USE_INSTANCING] = "QUAD_INST"
    #$global:techniques[($__SPHERE_TECH_BASE -shr 1) -bor $SPHERE_TECHNIQUE_USE_SRV -bor $SPHERE_TECHNIQUE_USE_RAYCASTING -bor $SPHERE_TECHNIQUE_USE_INSTANCING] = "POLY_INST"
    $global:techniques[($__SPHERE_TECH_BASE -shr 2) -bor $SPHERE_TECHNIQUE_USE_TESS -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "QUAD_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 3) -bor $SPHERE_TECHNIQUE_USE_TESS -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "POLY_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 4) -bor $SPHERE_TECHNIQUE_USE_TESS -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "ADAPT_POLY_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 5) -bor $SPHERE_TECHNIQUE_USE_GEO -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "STPA"
    $global:techniques[($__SPHERE_TECH_BASE -shr 6) -bor $SPHERE_TECHNIQUE_USE_GEO -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "GEO_QUAD"
    $global:techniques[($__SPHERE_TECH_BASE -shr 7) -bor $SPHERE_TECHNIQUE_USE_GEO -bor $SPHERE_TECHNIQUE_USE_RAYCASTING] = "GEO_POLY"
    $global:techniques[($__SPHERE_TECH_BASE -shr 8) -bor $SPHERE_TECHNIQUE_USE_TESS] = "SPHERE_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 9) -bor $SPHERE_TECHNIQUE_USE_TESS] = "ADAPT_SPHERE_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 10) -bor $SPHERE_TECHNIQUE_USE_TESS] = "HEMISPHERE_TESS"
    $global:techniques[($__SPHERE_TECH_BASE -shr 11) -bor $SPHERE_TECHNIQUE_USE_TESS] = "ADAPT_HEMISPHERE_TESS"
    # Note: Number of techniques must be below 32!

    # Properties of the input data.
    $SPHERE_INPUT_PV_COLOUR = ([uint64] 1) -shl 0
    $SPHERE_INPUT_PV_RADIUS = ([uint64] 1) -shl 1
    $SPHERE_INPUT_PV_INTENSITY = ([uint64] 1) -shl 2
    $SPHERE_INPUT_PP_INTENSITY = ([uint64] 1) -shl 16
    $SPHERE_INPUT_FLT_COLOUR = ([uint64] 1) -shl 17

    # Variants of the renderer.
    $SPHERE_VARIANT_PV_RAY = ([uint64] 1) -shl 18
    $SPHERE_VARIANT_CONSERVATIVE_DEPTH = ([uint64] 1) -shl 19

    # List of all techniques that have been created.
    $shaders = @{}

    # Creates the wrapper of a single shader based on the given core
    # implementation.
    function Create-Shader([string] $fileBase, [string] $core, [uint64] $technique, [uint64] $features) {
        $id = ($technique -bor $features)
        $retval = "$fileBase$("{0:X16}" -f $id)"

        $lines = @()
        $lines += "// This file was auto-generated using Create-SphereShaders.ps1 on $(Get-Date)"

        $lines += "#define $($global:techniques[$technique]) (1)"


        if ($features -band $SPHERE_INPUT_PV_COLOUR) {
            $lines += '#define PER_VERTEX_COLOUR (1)'
        }
        if ($features -band $SPHERE_INPUT_FLT_COLOUR) {
            $lines += '#define FLOAT_COLOUR (1)'
        }
        if ($features -band $SPHERE_INPUT_PV_RADIUS) {
            $lines += '#define PER_VERTEX_RADIUS (1)'
        }
        if ($features -band $SPHERE_INPUT_PV_INTENSITY) {
            $lines += '#define PER_VERTEX_INTENSITY (1)'
        }
        if ($features -band $SPHERE_INPUT_PP_INTENSITY) {
            $lines += '#define PER_PIXEL_INTENSITY (1)'
        }
        if ($features -band $SPHERE_VARIANT_PV_RAY) {
            $lines += '#define PER_VERTEX_RAY (1)'
        }
        if ($technique -band $SPHERE_TECHNIQUE_USE_RAYCASTING) {
            $lines += '#define RAYCASTING (1)'
        }
        if ($technique -band $SPHERE_TECHNIQUE_USE_INSTANCING) {
            $lines += '#define INSTANCING (1)'
        }
        if ($technique -band $SPHERE_VARIANT_CONSERVATIVE_DEPTH) {
            $lines += '#define CONSERVATIVE_DEPTH (1)'
        }

        $lines += "#include `"$core`""

        $file = "$retval.hlsl"
        $file = Join-Path $OutPath $file
        $lines | Out-File -FilePath $file -Encoding ascii

        return $retval
    }
}

process {
    0..1 | %{
        $pvRadius = $_ * $SPHERE_INPUT_PV_RADIUS
        
        0..2 | %{
            $pvColour = ([int] ($_ / 2)) * $SPHERE_INPUT_PV_COLOUR
            $pvIntensity = ($_ % 2) * $SPHERE_INPUT_PV_INTENSITY
            #echo "RAD $pvRadius"
            #echo "COL $pvColour"
            #echo "INT $pvIntensity"

            $cntXfer = 0
            if ($pvIntensity -ne 0) {
                # Technique has per-vertex intensity, so we need to test VS
                # texture lookup and PS texture lookup.
                $cntXfer = 1
            }

            0..$cntXfer | %{
                $xfer = ($_ * $SPHERE_INPUT_PP_INTENSITY) -bor ((1 - $_) * $pvIntensity)
                                
                $global:techniques.Keys | %{
                    $technique = $_
                
                    $cntFlt = 0
                    if (($technique -band $SPHERE_TECHNIQUE_USE_SRV) -and ($pvColour -ne 0)) {
                        # Technique uses structured resource view and format
                        # includes colour, so we need to include in-shader RGB8 to 
                        # float conversion and host code conversion.
                        $cntFlt = 1
                    }

                    $cntRay = 0
                    $cntConvDepth = 0
                    if ($technique -band $SPHERE_TECHNIQUE_USE_RAYCASTING) {
                        # If the technique uses raycasting, test per-vertex and
                        # per-pixel generation of rays. Also, raycasting supports
                        # optional conservative depth.
                        $cntRay = 1
                        $cntConvDepth = 1
                    }

                    0..$cntFlt | %{
                        $flt = $_ * $SPHERE_INPUT_FLT_COLOUR


                        0..$cntRay | %{
                            $pvRay = $_ * $SPHERE_VARIANT_PV_RAY

                            0..$cntConvDepth | %{
                                $convDepth = $_ * $SPHERE_VARIANT_CONSERVATIVE_DEPTH

                                $features = ($pvRadius -bor $pvColour -bor $flt -bor $xfer -bor $pvRay -bor $convDepth)
                                $id = ($technique -bor $features)
                                $shader = New-Object PSObject
                        
                                $vs = Create-Shader "SphereVertexShader" "SphereVertexShaderCore.hlsli" $technique $features
                                $shader | Add-Member NoteProperty VertexShader $vs

                                if ($technique -band $SPHERE_TECHNIQUE_USE_TESS) {
                                    $hs = Create-Shader "SphereHullShader" "SphereHullShaderCore.hlsli" $technique $features
                                    $shader | Add-Member NoteProperty HullShader $hs
                                    $ds = Create-Shader "SphereDomainShader" "SphereDomainShaderCore.hlsli" $technique $features
                                    $shader | Add-Member NoteProperty DomainShader $ds
                                }

                                if ($technique -band $SPHERE_TECHNIQUE_USE_GEO) {
                                    $gs = Create-Shader "SphereGeometryShader" "SphereGeometryShaderCore.hlsli" $technique $features
                                    $shader | Add-Member NoteProperty GeometryShader $gs
                                }

                                $ps = Create-Shader "SpherePixelShader" "SpherePixelShaderCore.hlsli" $technique $features
                                $shader | Add-Member NoteProperty PixelShader $ps

                                $shaders[$id] = $shader
                            }
                        }
                    }
                }
            }
        }
    }


    # The lines of the include file to be generated.
    $includes = @()
    $includes += "// This file was auto-generated using Create-SphereShaders.ps1 on $(Get-Date)"

    $includes += "#define SPHERE_TECHNIQUE_USE_GEO ($("0x{0:X}" -f $SPHERE_TECHNIQUE_USE_GEO))"
    $includes += "#define SPHERE_TECHNIQUE_USE_TESS ($("0x{0:X}" -f $SPHERE_TECHNIQUE_USE_TESS))"
    $includes += "#define SPHERE_TECHNIQUE_USE_SRV ($("0x{0:X}" -f $SPHERE_TECHNIQUE_USE_SRV))"
    $includes += "#define SPHERE_TECHNIQUE_USE_RAYCASTING ($("0x{0:X}" -f $SPHERE_TECHNIQUE_USE_RAYCASTING))"
    $includes += "#define SPHERE_TECHNIQUE_USE_INSTANCING ($("0x{0:X}" -f $SPHERE_TECHNIQUE_USE_INSTANCING))"
    $includes += "#define SPHERE_INPUT_PV_COLOUR ($("0x{0:X}" -f $SPHERE_INPUT_PV_COLOUR))"
    $includes += "#define SPHERE_INPUT_PV_RADIUS ($("0x{0:X}" -f $SPHERE_INPUT_PV_RADIUS))"
    $includes += "#define SPHERE_INPUT_PV_INTENSITY ($("0x{0:X}" -f $SPHERE_INPUT_PV_INTENSITY))"
    $includes += "#define SPHERE_INPUT_PP_INTENSITY ($("0x{0:X}" -f $SPHERE_INPUT_PP_INTENSITY))"
    $includes += "#define SPHERE_INPUT_FLT_COLOUR ($("0x{0:X}" -f $SPHERE_INPUT_FLT_COLOUR))"
    $includes += "#define SPHERE_VARIANT_PV_RAY ($("0x{0:X}" -f $SPHERE_VARIANT_PV_RAY))"
    $includes += "#define SPHERE_VARIANT_CONSERVATIVE_DEPTH ($("0x{0:X}" -f $SPHERE_VARIANT_CONSERVATIVE_DEPTH))"

    $includes += "#define _DEFINE_SPHERE_TECHNIQUE_LUT(n) static const struct { const char *name; std::uint64_t id; } n[] = {\"
    $global:techniques.Keys | %{
        $includes +="{ `"$($global:techniques[$_].ToLower())`", $_ },\"
    }
    $includes += "{ nullptr, 0 } }"

    # The lines of the resource script to be generated.
    $resources = @()
    $resources += "// This file was auto-generated using Create-SphereShaders.ps1 on $(Get-Date)"
    #$resources += "#include `"$IncludeFile`""

    # The current resource ID.
    $resID = $ResourceStart


    $includes += "#define _ADD_SPHERE_SHADERS(ht)\"
    $shaders.Keys | %{
        $isFirst = ($resID -eq $ResourceStart)
        $id = $_
        $shader = $shaders[$id]
        $vs = $shader.VertexShader
        $hs = $shader.HullShader
        $ds = $shader.DomainShader
        $gs = $shader.GeometryShader
        $ps = $shader.PixelShader

        if ($vs) {
            $resources += "$resID $ResourceType $vs.cso"
            $vs = $resID++
        } else {
            $vs = 0
        }

        if ($hs) {
            $resources += "$resID $ResourceType $hs.cso"
            $hs = $resID++
        } else {
            $hs = 0
        }

        if ($ds) {
            $resources += "$resID $ResourceType $ds.cso"
            $ds = $resID++
        } else {
            $ds = 0
        }

        if ($gs) {
            $resources += "$resID $ResourceType $gs.cso"
            $gs = $resID++
        } else {
            $gs = 0
        }

        if ($ps) {
            $resources += "$resID $ResourceType $ps.cso"
            $ps = $resID++
        } else {
            $ps = 0
        }

        if (-not $isFirst) {
            $includes[-1] += ";\"
        }
        $includes += "ht[$id] = { $vs, $hs, $ds, $gs, $ps }"
    }

    $includes | Out-File -FilePath (Join-Path $OutPath $IncludeFile) -Encoding ascii 
    $resources | Out-File -FilePath (Join-Path $OutPath $ResourceFile) -Encoding ascii

    Write-Verbose "$($shaders.Count) have been generated."
}

end { }

