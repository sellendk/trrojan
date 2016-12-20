/// <copyright file="configuration.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <vector>

#include "trrojan/named_variant.h"


namespace trrojan {

    /// <summary>
    /// A configuration, which is defined as a set of manifestations of
    /// (named) factors.
    /// </summary>
    typedef std::vector<named_variant> configuration;

}
