// <copyright file="empty_benchmark.cpp" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2022 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// Licensed under the MIT licence. See LICENCE.txt file in the project root for full licence information.
// </copyright>
// <author>Christoph M�ller</author>

#include "trrojan/d3d12/empty_benchmark.h"

#include <array>

#include "trrojan/result.h"
#include "trrojan/timer.h"

#include "trrojan/d3d12/gpu_timer.h"


#define _EMPTY_BENCH_DEFINE_FACTOR(f)                                         \
const char *trrojan::d3d12::empty_benchmark::factor_##f = #f

_EMPTY_BENCH_DEFINE_FACTOR(clear_colour);

#undef _EMPTY_BENCH_DEFINE_FACTOR


/*
 * trrojan::d3d12::empty_benchmark::empty_benchmark
 */
trrojan::d3d12::empty_benchmark::empty_benchmark(void) : benchmark_base("empty") {
    // Declare the configuration data we need to have.
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_clear_colour, std::array<float, 4> { 0.0f, 0.0f, 0.0f, 0.0f}));
}


/*
 * trrojan::d3d12::empty_benchmark::~empty_benchmark
 */
trrojan::d3d12::empty_benchmark::~empty_benchmark(void) { }


/*
 * trrojan::result trrojan::d3d12::empty_benchmark::on_run
 */
trrojan::result trrojan::d3d12::empty_benchmark::on_run(d3d12::device& device,
        const UINT current_frame, const UINT total_frames,
        const configuration& config, const std::vector<std::string>& changed) {
    auto clear_colour = config.get<std::array<float, 4>>(factor_clear_colour);
    auto cmd_list = device.create_graphics_command_list(
        D3D12_COMMAND_LIST_TYPE_DIRECT);
    timer cpu_timer;
    auto gpu_freq = gpu_timer::get_timestamp_frequency(device.command_queue());
    gpu_timer gpu_timer(device.d3d_device(), 2, 1);

    // Prepare the result set.
    auto retval = std::make_shared<basic_result>(config,
        std::initializer_list<std::string> {
            "clear_colour",
            "clear_time",
            "total_gpu_time",
    });

    // Record the command list.
    cpu_timer.start();
    gpu_timer.start_frame();

    gpu_timer.start(cmd_list, 0);

    gpu_timer.start(cmd_list, 1);
    this->clear_target(clear_colour, cmd_list);
    gpu_timer.end(cmd_list, 1);

    gpu_timer.end(cmd_list, 0);

    this->disable_target(cmd_list);
    auto result_index = gpu_timer.end_frame(cmd_list);

    // Run the benchmark.
    cmd_list->Close();
    device.execute_command_list(cmd_list);

    // Present and prepare the next frame.
    this->present_target();

    // Collect the results.
    retval->add({
        clear_colour,
        gpu_timer::to_milliseconds(gpu_timer.evaluate(result_index, 1), gpu_freq),
        gpu_timer::to_milliseconds(gpu_timer.evaluate(result_index, 0), gpu_freq)
        });

    return retval;
}
