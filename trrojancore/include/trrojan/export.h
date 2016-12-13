/// <copyright file="export.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once


#ifdef TRROJANCORE_EXPORTS
#define TRROJANCORE_API __declspec(dllexport)
#else /* TRROJANCORE_EXPORTS */
#define TRROJANCORE_API __declspec(dllimport)
#endif /* TRROJANCORE_EXPORTS*/
