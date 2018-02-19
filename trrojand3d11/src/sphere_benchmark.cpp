/// <copyright file="sphere_benchmark.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/d3d11/sphere_benchmark.h"

#include <cassert>
#include <cinttypes>
#include <memory>

#include <glm/ext.hpp>

#include "trrojan/constants.h"
#include "trrojan/factor_enum.h"
#include "trrojan/factor_range.h"
#include "trrojan/log.h"
#include "trrojan/mmpld_reader.h"
#include "trrojan/result.h"
#include "trrojan/system_factors.h"
#include "trrojan/timer.h"

#include "trrojan/d3d11/plugin.h"
#include "trrojan/d3d11/utilities.h"

#include "sphere_techniques.h"
#include "SpherePipeline.hlsli"


#define _SPHERE_BENCH_DEFINE_FACTOR(f)                                         \
const char *trrojan::d3d11::sphere_benchmark::factor_##f = #f

_SPHERE_BENCH_DEFINE_FACTOR(conservative_depth);
_SPHERE_BENCH_DEFINE_FACTOR(data_set);
_SPHERE_BENCH_DEFINE_FACTOR(edge_tess_factor);
_SPHERE_BENCH_DEFINE_FACTOR(force_float_colour);
_SPHERE_BENCH_DEFINE_FACTOR(frame);
_SPHERE_BENCH_DEFINE_FACTOR(inside_tess_factor);
_SPHERE_BENCH_DEFINE_FACTOR(iterations);
_SPHERE_BENCH_DEFINE_FACTOR(method);
_SPHERE_BENCH_DEFINE_FACTOR(poly_maximum);
_SPHERE_BENCH_DEFINE_FACTOR(poly_minimum);
_SPHERE_BENCH_DEFINE_FACTOR(poly_scale);
_SPHERE_BENCH_DEFINE_FACTOR(vs_raygen);
_SPHERE_BENCH_DEFINE_FACTOR(vs_xfer_function);

#undef _SPHERE_BENCH_DEFINE_FACTOR


_DEFINE_SPHERE_TECHNIQUE_LUT(SPHERE_METHODS);


/*
 * trrojan::d3d11::sphere_benchmark::sphere_benchmark
 */
trrojan::d3d11::sphere_benchmark::sphere_benchmark(void)
        : benchmark_base("sphere-renderer"), data_properties(0) {
    typedef mmpld_reader::shader_properties sp_t;

    // Define the data we need.
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_conservative_depth, { true, false }));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_edge_tess_factor, { std::array<float, 4> { 4, 4, 4, 4} }));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_force_float_colour, { true, false }));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_frame, static_cast<frame_type>(0)));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_inside_tess_factor, { std::array<float, 2> { 4, 4 } }));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_iterations, static_cast<unsigned int>(8)));
    {
        std::vector<std::string> manifestations;
        for (size_t i = 0; (::SPHERE_METHODS[i].name != nullptr); ++i) {
            manifestations.emplace_back(::SPHERE_METHODS[i].name);
        }
        this->_default_configs.add_factor(factor::from_manifestations(
            factor_method, manifestations));
    }
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_poly_maximum, static_cast<unsigned int>(8)));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_poly_minimum, static_cast<unsigned int>(4)));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_poly_scale, 2.0f));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_vs_raygen, { true, false }));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_vs_xfer_function, { true, false }));

    this->_default_configs.add_factor(factor::from_manifestations(
        factor_manoeuvre, manoeuvre_type("diagonal")));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_manoeuvre_step, static_cast<manoeuvre_step_type>(0)));
    this->_default_configs.add_factor(factor::from_manifestations(
        factor_manoeuvre_steps, static_cast<manoeuvre_step_type>(64)));

    // Build the lookup table for the shader resources.
    _ADD_SPHERE_SHADERS(this->shader_resources);
}


/*
 * trrojan::d3d11::sphere_benchmark::~sphere_benchmark
 */
trrojan::d3d11::sphere_benchmark::~sphere_benchmark(void) { }


/*
 * trrojan::d3d11::sphere_benchmark::optimise_order
 */
void trrojan::d3d11::sphere_benchmark::optimise_order(
        configuration_set& inOutConfs) {
    inOutConfs.optimise_order({ factor_data_set, factor_frame, factor_device });
}


/*
 * trrojan::d3d11::sphere_benchmark::required_factors
 */
std::vector<std::string> trrojan::d3d11::sphere_benchmark::required_factors(
        void) const {
    static const std::vector<std::string> retval = { factor_data_set,
        factor_device };
    return retval;
}


/*
 * trrojan::d3d11::sphere_benchmark::on_run
 */
trrojan::result trrojan::d3d11::sphere_benchmark::on_run(d3d11::device& device,
        const configuration& config, const std::vector<std::string>& changed) {
    trrojan::timer cpuTimer;
    const auto cntIterations = config.get<int>(factor_iterations);
    auto ctx = device.d3d_context();
    auto dev = device.d3d_device();
    gpu_timer_type::value_type gpuFreq;
    gpu_timer_type gpuTimer;
    auto isDisjoint = true;
    auto isRandomSpheres = false;
    auto shaderCode = sphere_benchmark::get_shader_id(config);
    SphereConstants sphereConstants;
    TessellationConstants tessConstants;
    ViewConstants viewConstants;
    D3D11_VIEWPORT viewport;

    // If the device has changed, invalidate all GPU resources and recreate the
    // data-independent ones.
    if (contains(changed, factor_device)) {
        log::instance().write_line(log_level::verbose, "Preparing GPU "
            "resources for device \"%s\" ...", device.name().c_str());
        this->colour_map = nullptr;
        this->data_buffer = nullptr;
        this->data_properties = 0;
        this->done_query = nullptr;
        this->linear_sampler = nullptr;
        this->stats_query = nullptr;
        this->technique_cache.clear();

        // Textures and SRVs
        create_viridis_colour_map(dev, &this->colour_map);

        // Samplers
        this->linear_sampler = create_linear_sampler(dev);

        // Queries
        this->done_query = create_event_query(dev);
        this->stats_query = create_pipline_stats_query(dev);
    }

    // Determine whether the data set header must be loaded. This needs to be
    // done before any frame is loaded or the technique is selected.
    if (contains(changed, factor_data_set)) {
        // Invalidate previous data.
        this->data_buffer = nullptr;
        this->data_properties = 0;

        try {
            this->try_make_random_spheres(dev, shaderCode, config);
            isRandomSpheres = true;
        } catch (std::exception& ex) {
            log::instance().write_line(log_level::warning, ex);
            isRandomSpheres = false;

            auto path = config.get<std::string>(factor_data_set);
            log::instance().write_line(log_level::verbose, "Loading MMPLD data "
                "set \"%s\" ...", path.c_str());
            this->open_mmpld(path.c_str());
        }
    }
    /* At this point, the data header for processing the MMPLD is OK. */

    // In case we still have a data set, find out whether it is compatible with
    // the new shader code and erase it otherwise.
    if (this->data_buffer != nullptr) {
        // Frame must not have changed for data to be still valid. Note that
        // random spheres do not have frames, so we ignore the factor in this
        // case.
        auto isValid = isRandomSpheres || !contains(changed, factor_frame);

        // Type of buffer (VB or structured data) required must be the same.
        isValid = isValid && ((shaderCode & SPHERE_TECHNIQUE_USE_SRV)
            == (this->data_properties & SPHERE_TECHNIQUE_USE_SRV));

        // If there are per-vertex colour values and floating point colours are
        // enforced, the data must be float.
        if (config.get<bool>(factor_force_float_colour)
                && ((this->data_properties & SPHERE_INPUT_PV_COLOUR) != 0)) {
            isValid = isValid
                && ((this->data_properties & SPHERE_INPUT_FLT_COLOUR) != 0);
        }

        // If the existing data do not match, discard them.
        if (!isValid) {
            assert(!isRandomSpheres);   // Must be valid by design.
            this->data_buffer = nullptr;
            this->data_properties = 0;
        }
    }

    // (Re-) read MMPLD frame as necessary.
    if (this->data_buffer == nullptr) {
        auto frame = config.get<frame_type>(factor_frame);
        auto options = std::underlying_type<mmpld_loader_options>::type(0);
        if ((shaderCode & SPHERE_TECHNIQUE_USE_SRV) != 0) {
            options |= mmpld_loader_options::structured_resource;
            this->data_properties |= SPHERE_TECHNIQUE_USE_SRV;
        } else {
            options |= mmpld_loader_options::vertex_buffer;
            assert((this->data_properties & SPHERE_TECHNIQUE_USE_SRV) == 0);
        }
        if (config.get<bool>(factor_force_float_colour)) {
            options |= mmpld_loader_options::force_float_colour;
        }

        log::instance().write_line(log_level::verbose, "Loading MMPLD frame "
            "%u ...", frame);
        this->data_buffer = this->read_mmpld_frame(dev, frame,
            static_cast<mmpld_loader_options>(options));

        // Merge the requested data properties with the data-defined ones.
        this->data_properties |= this->get_mmpld_input_properties();
        this->set_float_colour_flag(this->mmpld_list.colour_type);
    }
    /* At this point, 'data_buffer' holds data compatible with renderer. */

    // Convert per-vertex TF lookup to per-pixel lookup if requested.
    if (!config.get<bool>(factor_vs_xfer_function)) {
        typedef mmpld_reader::shader_properties sp_t;
        if ((this->data_properties & sp_t::per_vertex_intensity) != 0) {
            this->data_properties &= ~sp_t::per_vertex_intensity;
            this->data_properties |= SPHERE_INPUT_PP_INTENSITY;
            log::instance().write_line(log_level::verbose, "Converted "
                "per-vertex intensity lookup to per-pixel lookup. The new "
                "data code is %u.", this->data_properties);
        }
    }
    /* At this point, we know what shader features are needed for the data. */

    // Select or create the right rendering technique.
    auto& technique = this->get_technique(dev, shaderCode,
        this->data_properties, isRandomSpheres);
    // TODO



#if 0


    /* Update all data-dependent resources. */
    if (contains_any(changed, factor_frame, factor_data_set, factor_device,
            factor_vs_xfer_function)) {
        auto psProps = this->get_mmpld_pixel_shader_properties(
            isVsXferFunction);
        auto psCode = this->pixel_shaders.find(psProps);
        if (psCode == this->pixel_shaders.end()) {
            throw std::runtime_error("No pixel shader is available which can "
                "fulfil the requirements of the data set.");
        }

        auto vsProps = this->get_mmpld_vertex_shader_properties(
            isVsXferFunction);
        auto vsCode = this->vertex_shaders.find(vsProps);
        if (vsCode == this->vertex_shaders.end()) {
            throw std::runtime_error("No vertex shader is available which can "
                "fulfil the requirements of the data set.");
        }

        this->input_layout = create_input_layout(device.d3d_device(),
            this->mmpld_layout, vsCode->second.first, vsCode->second.second);

        this->vertex_shader = create_vertex_shader(device.d3d_device(),
            vsCode->second.first, vsCode->second.second);

        this->pixel_shader = create_pixel_shader(device.d3d_device(),
            psCode->second.first, psCode->second.second);
    }

    /* Retrieve the viewport for rasteriser and shaders. */
    {
        auto vp = config.get<viewport_type>(factor_viewport);
        viewport.TopLeftX = viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(vp[0]);
        viewport.Height = static_cast<float>(vp[1]);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        constants.Viewport.x = 0.0f;
        constants.Viewport.y = 0.0f;
        constants.Viewport.z = viewport.Width;
        constants.Viewport.w = viewport.Height;
    }

    // Initialise the GPU timer.
    gpuTimer.initialise(dev);

    // Prepare the result set.
    auto retval = std::make_shared<basic_result>(std::move(config),
        std::initializer_list<std::string> {
            "iteration",
            "particles",
            "vs_invokes",
            "gs_invokes",
            "ps_invokes",
            "gpu_time",
            "wall_time"
    });

    /* Compute the matrices. */
    {
        //auto projection = DirectX::XMMatrixPerspectiveFovRH(std::atan(1) * 4 / 3,
        //    static_cast<float>(viewport.Width) / static_cast<float>(viewport.Height),
        //    0.1f, 10.0f);

        //auto eye = DirectX::XMFLOAT4(0, 0, 0.5f * (this->mmpld_header.bounding_box[5] - this->mmpld_header.bounding_box[2]), 0);
        //auto lookAt = DirectX::XMFLOAT4(0, 0, 0, 0);
        //auto up = DirectX::XMFLOAT4(0, 1, 0, 0);
        //auto view = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat4(&eye),
        //    DirectX::XMLoadFloat4(&lookAt), DirectX::XMLoadFloat4(&up));

        this->cam.set_fovy(60.0f);
        this->cam.set_aspect_ratio(static_cast<float>(viewport.Width) / static_cast<float>(viewport.Height));
        auto mat = DirectX::XMFLOAT4X4(glm::value_ptr(this->cam.get_projection_mx()));
        auto projection = DirectX::XMLoadFloat4x4(&mat);
        DirectX::XMStoreFloat4x4(constants.ProjMatrix,
            DirectX::XMMatrixTranspose(projection));

        auto bbWidth = std::abs(this->mmpld_header.bounding_box[3]
            - this->mmpld_header.bounding_box[0]);
        auto bbHeight = std::abs(this->mmpld_header.bounding_box[4]
            - this->mmpld_header.bounding_box[1]);
        auto bbDepth = std::abs(this->mmpld_header.bounding_box[5]
            - this->mmpld_header.bounding_box[2]);
        auto bbMax = (std::max)(bbWidth, bbHeight);
        auto dist = 0.5f * bbMax / std::tan(this->cam.get_fovy() / 180.f
            * trrojan::constants<float>::pi);

        this->cam.set_near_plane_dist(0.1f);
        this->cam.set_far_plane_dist(2.0f * bbMax);

        auto bbStartX = std::min(this->mmpld_header.bounding_box[3],
            this->mmpld_header.bounding_box[0]);
        auto bbStartY = std::min(this->mmpld_header.bounding_box[4],
            this->mmpld_header.bounding_box[1]);
        auto bbStartZ = std::min(this->mmpld_header.bounding_box[5],
            this->mmpld_header.bounding_box[2]);

        auto pos = glm::vec3(bbStartX + 0.5f * bbWidth,
            bbStartY + 0.5f * bbHeight,
            bbStartZ + 0.5f * bbDepth + dist);
        auto lookAt = pos + glm::vec3(0.0f, 0.0f, 0.5f * bbDepth);

        this->cam.set_look(pos, lookAt, glm::vec3(0, 1, 0));

        point_type bbs, bbe;
        this->get_mmpld_bounding_box(bbs, bbe);
        this->apply_manoeuvre(this->cam, config, bbs, bbe);

        auto clipping = this->get_mmpld_clipping(cam);
        this->cam.set_near_plane_dist(clipping.first);
        this->cam.set_far_plane_dist(clipping.second);


        mat = DirectX::XMFLOAT4X4(glm::value_ptr(this->cam.get_view_mx()));
        auto view = DirectX::XMLoadFloat4x4(&mat);
        DirectX::XMStoreFloat4x4(constants.ViewMatrix,
            DirectX::XMMatrixTranspose(view));

        auto viewDet = DirectX::XMMatrixDeterminant(view);
        auto viewInv = DirectX::XMMatrixInverse(&viewDet, view);
        DirectX::XMStoreFloat4x4(constants.ViewInvMatrix,
            DirectX::XMMatrixTranspose(viewInv));

        auto viewProj = view * projection;
        DirectX::XMStoreFloat4x4(constants.ViewProjMatrix,
            DirectX::XMMatrixTranspose(viewProj));

        auto viewProjDet = DirectX::XMMatrixDeterminant(viewProj);
        auto viewProjInv = DirectX::XMMatrixInverse(&viewProjDet, viewProj);
        DirectX::XMStoreFloat4x4(constants.ViewProjInvMatrix,
            DirectX::XMMatrixTranspose(viewProjInv));
    }

    /* Compute shader constants besides matrices. */
    constants.GlobalColour.x = this->mmpld_list.colour[0];
    constants.GlobalColour.y = this->mmpld_list.colour[1];
    constants.GlobalColour.z = this->mmpld_list.colour[2];
    constants.GlobalColour.w = this->mmpld_list.colour[3];

    constants.IntensityRange.x = this->mmpld_list.min_intensity;
    constants.IntensityRange.y = this->mmpld_list.max_intensity;
    constants.GlobalRadius = this->mmpld_list.radius;
    constants.IntRangeGlobalRadTessFactor.w = 5.0f;

    /* Update constant buffer. */
    ctx->UpdateSubresource(this->constant_buffer.p, 0, nullptr,
        &constants, 0, 0);

    /* Configure vertex stage. */
    ctx->VSSetShader(this->vertex_shader.p, nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &this->constant_buffer.p);
    ctx->VSSetShaderResources(0, 1, &this->colour_map.p);
    ctx->VSSetSamplers(0, 1, &this->linear_sampler.p);

    if (isTessellation) {
        /* Configure hull shader stage. */
        ctx->HSSetShader(this->hull_shader.p, nullptr, 0);
        ctx->HSSetConstantBuffers(0, 1, &this->constant_buffer.p);

        /* Configure domain shader stage. */
        ctx->DSSetShader(this->domain_shader.p, nullptr, 0);
        ctx->DSSetConstantBuffers(0, 1, &this->constant_buffer.p);
    } else {
        ctx->HSSetShader(nullptr, nullptr, 0);
        ctx->DSSetShader(nullptr, nullptr, 0);
    }

    if (!isTessellation) {
        /* Configure geometry stage. */
        ctx->GSSetShader(this->geometry_shader.p, nullptr, 0);
        ctx->GSSetConstantBuffers(0, 1, &this->constant_buffer.p);
    } else {
        ctx->GSSetShader(nullptr, nullptr, 0);
    }

    /* Configure pixel stage. */
    ctx->PSSetShader(this->pixel_shader.p, nullptr, 0);
    ctx->PSSetConstantBuffers(0, 1, &this->constant_buffer.p);
    ctx->PSSetShaderResources(0, 1, &this->colour_map.p);
    ctx->PSSetSamplers(0, 1, &this->linear_sampler.p);

    /* Set vertex buffer. */
    const auto offset = 0u;
    const auto stride = static_cast<UINT>(
        mmpld_reader::calc_stride(this->mmpld_list));
    const auto topology = isTessellation
        ? D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST
        : D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    ctx->IASetVertexBuffers(0, 1, &this->vertex_buffer.p, &stride, &offset);
    ctx->IASetPrimitiveTopology(topology);
    ctx->IASetInputLayout(this->input_layout.p);

    /* Configure the rasteriser. */
    ctx->RSSetViewports(1, &viewport);
    //context->RSSetState(this->rasteriserState.Get());

    /* Render it. */
    assert(this->mmpld_list.particles <= UINT_MAX);

    for (int i = 0; i < cntIterations;) {
        log::instance().write_line(log_level::debug, "Iteration %d.", i);
        cpuTimer.start();
        gpuTimer.start_frame();
        gpuTimer.start(0);
        this->clear_target();
        ctx->Begin(this->stats_query);
        ctx->Draw(static_cast<UINT>(this->mmpld_list.particles), 0);
        ctx->End(this->stats_query);
        this->present_target();
        ctx->End(this->done_query);
        gpuTimer.end(0);
        gpuTimer.end_frame();
        wait_for_event_query(ctx, this->done_query);
        auto cpuTime = cpuTimer.elapsed_millis();

        gpuTimer.evaluate_frame(isDisjoint, gpuFreq, 5 * 1000);
        if (!isDisjoint) {
            D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStats;
            auto hr = ctx->GetData(this->stats_query, &pipeStats, sizeof(pipeStats), 0);
            assert(SUCCEEDED(hr));

            auto gpuTime = gpu_timer_type::to_milliseconds(
                    gpuTimer.evaluate(0), gpuFreq);
            retval->add({ i,
                this->mmpld_list.particles,
                pipeStats.VSInvocations,
                pipeStats.GSInvocations,
                pipeStats.PSInvocations,
                gpuTime,
                cpuTime });
            ++i;
        }

        //this->save_target();    // TODO
    }

    return retval;
#endif

    throw 1;
}


/*
 * trrojan::d3d11::sphere_benchmark::get_shader_id
 */
trrojan::d3d11::sphere_benchmark::shader_id_type
trrojan::d3d11::sphere_benchmark::get_shader_id(const std::string& method) {
    for (size_t i = 0; (::SPHERE_METHODS[i].name != nullptr);++i) {
        if (method == ::SPHERE_METHODS[i].name) {
            return ::SPHERE_METHODS[i].id;
        }
    }
    /* Not found at this point. */

    return 0;
}


/*
 * trrojan::d3d11::sphere_benchmark::get_shader_id
 */
trrojan::d3d11::sphere_benchmark::shader_id_type
trrojan::d3d11::sphere_benchmark::get_shader_id(const configuration& config) {
    auto isConsDepth = config.get<bool>(factor_conservative_depth);
    auto isVsRay = config.get<bool>(factor_vs_raygen);
    auto method = config.get<std::string>(factor_method);

    auto retval = sphere_benchmark::get_shader_id(method);
    if ((retval & SPHERE_TECHNIQUE_USE_RAYCASTING) != 0) {
        if (isConsDepth) {
            retval |= SPHERE_VARIANT_CONSERVATIVE_DEPTH;
        }
        if (isVsRay) {
            retval |= SPHERE_VARIANT_PV_RAY;
        }
    }

    return retval;
}


/*
 * trrojan::d3d11::sphere_benchmark::get_technique
 */
trrojan::d3d11::rendering_technique&
trrojan::d3d11::sphere_benchmark::get_technique(ID3D11Device *device,
        const shader_id_type method, const shader_id_type data,
        const bool isRandomSpheres) {
    auto id = method | data;
    auto isPsTex = ((method & SPHERE_INPUT_PP_INTENSITY) != 0);
    auto isVsTex = ((method & SPHERE_INPUT_PV_INTENSITY) != 0);
    auto isSrvParts = ((method & SPHERE_TECHNIQUE_USE_SRV) != 0);
    auto isRay = ((method & SPHERE_TECHNIQUE_USE_RAYCASTING) != 0);
    auto isInst = ((method & SPHERE_TECHNIQUE_USE_INSTANCING) != 0);
    auto isTess = ((method & SPHERE_TECHNIQUE_USE_TESS) != 0);

    auto retval = this->technique_cache.find(method);
    if (retval == this->technique_cache.end()) {
        log::instance().write_line(log_level::verbose, "No cached sphere "
            "rendering technique for %" PRIu64 " with data features %" PRIu64
            " (ID %" PRIu64 ") was found. Creating a new one ...", method, data,
            id);
        rendering_technique::input_layout_type il = nullptr;
        rendering_technique::vertex_shader_type vs = nullptr;
        rendering_technique::hull_shader_type hs = nullptr;
        rendering_technique::domain_shader_type ds = nullptr;
        rendering_technique::geometry_shader_type gs = nullptr;
        rendering_technique::pixel_shader_type ps = nullptr;
        auto pt = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

        auto it = this->shader_resources.find(id);
        if (it == this->shader_resources.end()) {
            throw std::runtime_error("Shader sources for the given sphere "
                "rendering method ware missing.");
        }

        if (it->second.vertex_shader != 0) {
            auto src = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(it->second.vertex_shader), _T("SHADER"));
            vs = create_vertex_shader(device, src);
            il = create_input_layout(device,
                isRandomSpheres ? this->random_data_layout : this->mmpld_layout,
                src);
        }
        if (it->second.hull_shader != 0) {
            assert(isTess);
            auto src = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(it->second.hull_shader), _T("SHADER"));
            hs = create_hull_shader(device, src);
        }
        if (it->second.domain_shader != 0) {
            assert(isTess);
            auto src = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(it->second.domain_shader), _T("SHADER"));
            ds = create_domain_shader(device, src);
        }
        if (it->second.geometry_shader != 0) {
            auto src = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(it->second.geometry_shader), _T("SHADER"));
            gs = create_geometry_shader(device, src);
        }
        if (it->second.pixel_shader != 0) {
            auto src = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(it->second.pixel_shader), _T("SHADER"));
            ps = create_pixel_shader(device, src);
        }

        if (isTess) {
            // All tessellation-based techniques use 1 control point lists.
            pt = D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
        } else if (isRay && isInst) {
            // Raycasting and instances creates tri-strip from nothing.
            pt = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        } else if (isInst) {
            // Geometry instancing uses triangles and index buffers.
            pt = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        }

        //this->techniqueCache[id] = rendering_technique(method, nullptr, D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST, vs, );
        this->technique_cache[id] = rendering_technique(
            //std::to_string(id), il, );
        );
    }

    retval = this->technique_cache.find(id);
    return retval->second;
}


/*
 * trrojan::d3d11::sphere_benchmark::set_data_properties
 */
void trrojan::d3d11::sphere_benchmark::set_data_properties(
        const random_sphere_type type, const shader_id_type shaderCode) {
    switch (type) {
        case random_sphere_type::pos_intensity:
            this->data_properties |= SPHERE_INPUT_PV_INTENSITY;
            /* falls through. */
        case random_sphere_type::pos_rad_intensity:
            this->data_properties |= SPHERE_INPUT_PV_RADIUS;
            break;

        case random_sphere_type::pos_rgba8:
        case random_sphere_type::pos_rgba32:
            this->data_properties |= SPHERE_INPUT_PV_COLOUR;
            /* falls through. */
        case random_sphere_type::pos_rad_rgba8:
        case random_sphere_type::pos_rad_rgba32:
            this->data_properties |= SPHERE_INPUT_PV_RADIUS;
            break;
    }

    // Indicate that the buffer is for SRVs.
    this->data_properties |= (shaderCode & SPHERE_TECHNIQUE_USE_SRV);

    // See MMPLD overload for detailed explanation.
    if ((this->data_properties & SPHERE_TECHNIQUE_USE_SRV) != 0) {
        switch (type) {
            case random_sphere_type::pos_rad_rgba32:
            case random_sphere_type::pos_rgba32:
                this->data_properties |= SPHERE_INPUT_FLT_COLOUR;
                break;
        }
    }
}


/*
 * trrojan::d3d11::sphere_benchmark::set_float_colour_flag
 */
void trrojan::d3d11::sphere_benchmark::set_float_colour_flag(
        const mmpld_reader::colour_type colour) {
    // If the data are in a structured resource buffer, we need to remember
    // whether the data are floating point data or 8-bit colours. Otherwise,
    // this information is not required and also must not be stored, because
    // the 'data_properties' flags are used to lookup the shader, which does
    // not request the flags for vertex buffer input.
    if ((this->data_properties & SPHERE_TECHNIQUE_USE_SRV) != 0) {
        switch (colour) {
            case mmpld_reader::colour_type::float_rgb:
            case mmpld_reader::colour_type::float_rgba:
                this->data_properties |= SPHERE_INPUT_FLT_COLOUR;
                break;
        }
    }
}


/*
 * trrojan::d3d11::sphere_benchmark::try_make_random_spheres
 */
void trrojan::d3d11::sphere_benchmark::try_make_random_spheres(
        ID3D11Device *dev, const shader_id_type shaderCode,
        const configuration& config) {
    auto path = config.get<std::string>(factor_data_set);
    auto forceFloat = config.get<bool>(factor_force_float_colour);
    auto bufferType = ((shaderCode & SPHERE_TECHNIQUE_USE_SRV) != 0)
        ? buffer_type::structured_resource : buffer_type::vertex_buffer;

    this->data_buffer = this->make_random_spheres(dev, bufferType, path,
        forceFloat);
    this->set_data_properties(this->random_data_type, shaderCode);
}
