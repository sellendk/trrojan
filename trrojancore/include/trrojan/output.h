/// <copyright file="output.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 - 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <memory>

#include "trrojan/configuration.h"
#include "trrojan/export.h"
#include "trrojan/output_params.h"
#include "trrojan/result.h"


namespace trrojan {

    /// <summary>
    /// Base class for output handlers.
    /// </summary>
    class TRROJANCORE_API output_base {

    public:

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        virtual ~output_base(void);

        /// <summary>
        /// Closes the output channel.
        /// </summary>
        virtual void close(void) = 0;

        /// <summary>
        /// Opens the output channel for writing.
        /// </summary>
        virtual void open(const output_params& params) = 0;

        /// <summary>
        /// Stores the given benchmark <see cref="trrojan::result" /> in the
        /// output.
        /// </summary>
        /// <param name="result"></param>
        /// <returns><c>*this</c></returns>
        virtual output_base& operator <<(const basic_result& result) = 0;

        /// <summary>
        /// Stores the given benchmark <see cref="trrojan::result" /> in the
        /// output.
        /// </summary>
        /// <remarks>
        /// It is safe to pass <c>nullptr</c>, in which case nothing will
        /// happen.
        /// </remarks>
        /// <param name="result"></param>
        /// <returns><c>*this</c></returns>
        output_base& operator <<(const result result);

    protected:

        inline output_base(void) { }

        output_base(const output_base&) = delete;

        output_base& operator =(const output_base&) = delete;

    };

    /// <summary>
    /// An output handler.
    /// </summary>
    typedef std::shared_ptr<output_base> output;
}
