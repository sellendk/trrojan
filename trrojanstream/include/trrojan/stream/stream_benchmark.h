/// <copyright file="stream_benchmark.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include "trrojan/benchmark.h"

#include <atomic>
#include <memory>

#include "trrojan/stream/export.h"


namespace trrojan {
namespace stream {

    /// <summary>
    /// The implementation of the memory streaming benchmark.
    /// </summary>
    /// <remarks>
    /// The implementation of this benchmark is closely related to John
    /// McCalpin's STREAM benchmark (http://www.cs.virginia.edu/stream/), but
    /// uses a different multi-threading model, which hopefully minimises the
    /// variance between the starting time of different threads compared to an
    /// OpenMP-based implementation, which is also dependent on the runtime
    /// used for the test.
    /// </remarks>
    class TRROJANSTREAM_API stream_benchmark : public trrojan::benchmark_base {

    public:

        static const std::string access_pattern_contiguous;

        static const std::string access_pattern_interleaved;

        inline stream_benchmark(void) : trrojan::benchmark_base("stream") { }

        virtual ~stream_benchmark(void);

        virtual result_set run(const configuration_set& configs);

    private:

    };

}
}
