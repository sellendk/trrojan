/// <copyright file="benchmark_base.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/d3d11/benchmark_base.h"

#include "trrojan/factor.h"
#include "trrojan/log.h"

#include "trrojan/d3d11/bench_render_target.h"


#define _D3D_BENCH_DEFINE_FACTOR(f)                                         \
const std::string trrojan::d3d11::benchmark_base::factor_##f(#f)

_D3D_BENCH_DEFINE_FACTOR(debug_view);
_D3D_BENCH_DEFINE_FACTOR(viewport);

#undef _D3D_BENCH_DEFINE_FACTOR


/*
 * trrojan::d3d11::benchmark_base::~benchmark_base
 */
trrojan::d3d11::benchmark_base::~benchmark_base(void) { }


/*
 * trrojan::d3d11::benchmark_base::can_run
 */
bool trrojan::d3d11::benchmark_base::can_run(trrojan::environment env,
        trrojan::device device) const noexcept {
    auto d = std::dynamic_pointer_cast<trrojan::d3d11::device>(device);
    return (d != nullptr);
}


/*
 * trrojan::d3d11::benchmark_base::draw_debug_view
 */
void trrojan::d3d11::benchmark_base::draw_debug_view(
        ATL::CComPtr<ID3D11Device> device,
        ATL::CComPtr<ID3D11DeviceContext> deviceContext) {
    assert(device != nullptr);
    assert(deviceContext != nullptr);

    //deviceContext->ClearRenderTargetView()
}


/*
 * trrojan::d3d11::benchmark_base::on_debug_view_resized
 */
void trrojan::d3d11::benchmark_base::on_debug_view_resized(
        ATL::CComPtr<ID3D11Device> device,
        const unsigned int width, const unsigned int height) {
}


/*
 * trrojan::d3d11::benchmark_base::on_debug_view_resizing
 */
void trrojan::d3d11::benchmark_base::on_debug_view_resizing(void) {
}


/*
 * trrojan::d3d11::benchmark_base::run
 */
trrojan::result trrojan::d3d11::benchmark_base::run(const configuration& c) {
    std::vector<std::string> changed;
    this->check_changed_factors(c, std::back_inserter(changed));

    auto genericDev = c.get<trrojan::device>(factor_device);
    auto device = std::dynamic_pointer_cast<trrojan::d3d11::device>(genericDev);

    if (device == nullptr) {
        throw std::runtime_error("A configuration without a Direct3D device was "
            "passed to a Direct3D benchmark.");
    }

    // Check whether the device has been changed. This should always be done
    // first, because all GPU resources, which depend on the content of 'c',
    // depend on the device as their storage location.
    if (std::find(changed.begin(), changed.end(), factor_device)
            != changed.end()) {
        log::instance().write_line(log_level::verbose, "The D3D device has "
            "changed. Reallocating all graphics resources ...");
        this->render_target = std::make_shared<bench_render_target>(device);
        // If the device has changed, force the viewport to be re-created:
        changed.push_back(factor_viewport);
    }

    // Resize the render target if the viewport has changed.
    if (std::find(changed.begin(), changed.end(), factor_viewport)
            != changed.end()) {
        auto vp = c.get<viewport_type>(factor_viewport);
        log::instance().write_line(log_level::verbose, "Resizing the "
            "benchmarking render target to %d � %d px ...", vp[0], vp[1]);
        this->render_target->resize(vp[0], vp[1]);
    }

    // Find out whether we need to start the debug view.
    {
        auto dv = c.get<bool>(factor_debug_view);
        log::instance().write_line(log_level::verbose, "Configuring the "
            "debug view to be %s ...", dv ? "on" : "off");
        //if (true||dv) {
        //    if (this->debugView == nullptr) {
        //        this->debugView = std::make_shared<debug_view>();
        //    }
        //} else {
        //    this->debugView = nullptr;
        //}
    }

    this->render_target->clear();
    this->render_target->enable();
    return this->on_run(*device, c, changed);
}


/*
 * trrojan::d3d11::benchmark_base::contains
 */
bool trrojan::d3d11::benchmark_base::contains(const std::string& needle,
        const std::vector<std::string>& haystack) {
    auto it = std::find(haystack.begin(), haystack.end(), needle);
    return (it != haystack.end());
}


/*
 * trrojan::d3d11::benchmark_base::benchmark_base
 */
trrojan::d3d11::benchmark_base::benchmark_base(const std::string& name)
        : trrojan::benchmark_base(name) {
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_debug_view, false));

    {
        auto dftViewport = std::array<unsigned int, 2> { 1024, 1024 };
        this->_default_configs.add_factor(factor::from_manifestations(
            factor_viewport, dftViewport));
    }
}
