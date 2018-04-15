/// <copyright file="scripting_exception.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <stdexcept>

#if defined(WITH_CHAKRA)
#include "ChakraCore.h"
#endif /* defined(WITH_CHAKRA) */


namespace trrojan {

    /// <summary>
    /// An exception thrown if an error in the scripting enging occurs.
    /// </summary>
    /// <remarks>
    class scripting_exception: public std::exception {

    public:

        /// <summary>
        /// The type of the native error codes.
        /// </summary>
#if defined(WITH_CHAKRA)
        typedef JsErrorCode error_type;
#else /* defined(WITH_CHAKRA) */
        typedef int error_type;
#endif /* defined(WITH_CHAKRA) */

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        inline scripting_exception(const error_type errorCode, const char *msg)
            : std::exception(msg), errorCode(errorCode) { }

        /// <summary>
        /// Answer the error code.
        /// </summary>
        inline error_type error_code(void) const {
            return this->errorCode;
        }

    private:

        error_type errorCode;

    };
}
