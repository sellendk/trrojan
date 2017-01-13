/// <copyright file="stream_benchmark.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/stream/stream_benchmark.h"

#include <cinttypes>

#include "trrojan/factor_enum.h"
#include "trrojan/factor_range.h"
#include "trrojan/system_factors.h"
#include "trrojan/timer.h"

#include "trrojan/stream/worker_thread.h"

#include <iostream>


/*
 * trrojan::stream::stream_benchmark::access_pattern_contiguous
 */
const std::string trrojan::stream::stream_benchmark::access_pattern_contiguous(
    "contiguous");


/*
 * trrojan::stream::stream_benchmark::access_pattern_interleaved
 */
const std::string trrojan::stream::stream_benchmark::access_pattern_interleaved(
    "interleaved");


/*
 * trrojan::stream::stream_benchmark::stream_benchmark
 */
trrojan::stream::stream_benchmark::stream_benchmark(void)
        : trrojan::benchmark_base("stream") {
    // If no scalar is specified, use a magic number.
    this->_default_configs.add_factor(factor::from_manifestations(
        "scalar", 42));

    // If no access pattern is specified, test all.
    this->_default_configs.add_factor(factor::from_manifestations(
        "access_pattern", { access_pattern_contiguous,
        access_pattern_interleaved }));

    // If no number of iterations is specified, use a magic number.
    this->_default_configs.add_factor(factor::from_manifestations(
        "iterations", 10));

    // If no number of threads is specifed, use all possible values up
    // to the number of logical processors in the system.
    auto lc = system_factors::instance().logical_cores().as<uint32_t>();
    this->_default_configs.add_factor(factor::from_manifestations(
        "threads", { 1u, lc }));

    // If no problem size is given, test all all of them.
    this->_default_configs.add_factor(factor::from_manifestations(
        "problem_size", worker_thread::problem_sizes::to_vector()));
}


/*
 * trrojan::stream::stream_benchmark::~stream_benchmark
 */
trrojan::stream::stream_benchmark::~stream_benchmark(void) { }


/*
 * trrojan::stream::stream_benchmark::run
 */
trrojan::result_set trrojan::stream::stream_benchmark::run(
        const configuration_set& configs) {
    std::vector<std::string> changed;
    result_set retval;

    // Check that caller has provided all required factors.
    this->check_required_factors(configs);

    // Merge missing factors from default configuration.
    auto c = configs;
    c.merge(this->_default_configs, false);

    // Invoke each configuration.
    c.foreach_configuration([&](const trrojan::configuration& c) {
        //changed.clear();
        //this->check_changed_factors(c, std::back_inserter(changed));
        //this->log_run(c);
        //std::cout << std::endl;
        //retval.push_back(this->run(c));
        //return true;
        // TODO: optimise reallocs.
        try {
            this->log_run(c);
            retval.push_back(std::move(this->run(c)));
            return true;
        } catch (const std::exception& ex) {
            log::instance().write_line(ex);
            return false;
        }
    });

    return retval;
}


/*
 * trrojan::stream::stream_benchmark::run
 */
trrojan::result trrojan::stream::stream_benchmark::run(
        const configuration& config) {
    trrojan::result retval;

    // TODO: remove hack
    //std::cout << "here" << std::endl;
    auto p = std::make_shared<problem>(
        //static_cast<scalar_type>(config,
        scalar_type::float32,
        42,
        task_type::copy,
        access_pattern::contiguous,
        8000000,
        1);
    std::array<worker_thread::pointer_type, 1> t;
    t[0] = worker_thread::create(p, worker_thread::make_barrier(1), 0);
    worker_thread::join(t.begin(), t.end());


    return retval;
}


///*
// * trrojan::stream::stream_benchmark::run0
// */
//bool trrojan::stream::stream_benchmark::run0(const trrojan::configuration& cfg) {
//    std::vector<std::string> changed;
//    this->check_changed_factors(cfg, std::back_inserter(changed));
//
//    return true;
//}
