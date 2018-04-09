/// <copyright file="two_pass_volume_benchmark.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/d3d11/two_pass_volume_benchmark.h"

#include <DirectXMath.h>

#include <glm/ext.hpp>

#include "trrojan/d3d11/plugin.h"
#include "trrojan/d3d11/utilities.h"

#include "TwoPassVolumePipeline.hlsli"


/*
 * trrojan::d3d11::two_pass_volume_benchmark::two_pass_volume_benchmark
 */
trrojan::d3d11::two_pass_volume_benchmark::two_pass_volume_benchmark(void)
    : volume_benchmark_base("2pass-volume-renderer") { }


/*
 * trrojan::d3d11::two_pass_volume_benchmark::on_run
 */
trrojan::result trrojan::d3d11::two_pass_volume_benchmark::on_run(
        d3d11::device& device, const configuration& config,
        const std::vector<std::string>& changed) {
    volume_benchmark_base::on_run(device, config, changed);

    glm::vec3 bbe, bbs;
    const auto ctx = device.d3d_context();
    const auto dev = device.d3d_device();
    RaycastingConstants raycastingConstants;
    ViewConstants viewConstants;
    const auto vp = config.get<viewport_type>(factor_viewport);
    const auto volSize = this->calc_physical_volume_size();

    // Compute the bounding box in world space.
    this->calc_bounding_box(bbs, bbe);

    // If the device changed, re-create resources idependent from data and
    // viewport.
    if (contains(changed, factor_device)) {
        // Constant buffers.
        this->raycasting_constants = create_buffer(dev, D3D11_USAGE_DEFAULT,
            D3D11_BIND_CONSTANT_BUFFER, nullptr, sizeof(RaycastingConstants));
        set_debug_object_name(this->raycasting_constants.p,
            "raycasting_constants");
        this->view_constants = create_buffer(dev, D3D11_USAGE_DEFAULT,
            D3D11_BIND_CONSTANT_BUFFER, nullptr, sizeof(ViewConstants));
        set_debug_object_name(this->view_constants.p, "view_constants");

        // Rebuild the ray computation technique.
        {
            auto vss = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(vertex_shader_resource_id), _T("SHADER"));
            auto vs = create_vertex_shader(dev, vss);

            ATL::CComPtr<ID3D11Buffer> ib;
            ATL::CComPtr<ID3D11Buffer> vb;
            auto ils = create_cube(dev, &vb, &ib);
            auto il = create_input_layout(dev, ils, vss);

            auto pss = d3d11::plugin::load_resource(
                MAKEINTRESOURCE(pixel_shader_resource_id), _T("SHADER"));
            auto ps = create_pixel_shader(dev, pss);

            auto stride = static_cast<UINT>(sizeof(DirectX::XMFLOAT3));
            this->ray_technique = rendering_technique("rays",
                { rendering_technique::vertex_buffer(vb, 8, stride) },
                rendering_technique::index_buffer(ib), il.p,
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                vs.p, rendering_technique::shader_resources(),
                ps.p, rendering_technique::shader_resources());

            this->ray_technique.set_constant_buffers(this->view_constants,
                static_cast<rendering_technique::shader_stages>(
                rendering_technique::shader_stage::vertex));
        }

        // Add a rasteriser state without backface culling.
        {
            ATL::CComPtr<ID3D11RasterizerState> state;

            D3D11_RASTERIZER_DESC desc;
            ::ZeroMemory(&desc, sizeof(desc));
            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = D3D11_CULL_NONE;

            auto hr = dev->CreateRasterizerState(&desc, &state);
            if (FAILED(hr)) {
                throw ATL::CAtlException(hr); 
            }
            this->ray_technique.set_rasteriser_state(state);
        }

        // Set an additive blending state for the ray computation pass.
        {
            ATL::CComPtr<ID3D11BlendState> state;

            D3D11_BLEND_DESC desc;
            ::ZeroMemory(&desc, sizeof(desc));
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            auto hr = dev->CreateBlendState(&desc, &state);
            if (FAILED(hr)) {
                throw ATL::CAtlException(hr);
            }
            this->ray_technique.set_blend_state(state);
        }

        // Rebuild the raycasting technique.
        {
            auto src = d3d11::plugin::load_resource(MAKEINTRESOURCE(
                compute_shader_resource_id), _T("SHADER"));
            auto cs = create_compute_shader(dev, src);
            auto res = rendering_technique::shader_resources();
            res.sampler_states.push_back(this->linear_sampler);
            res.resource_views.push_back(this->data_view);
            res.resource_views.push_back(this->xfer_func_view);
            res.constant_buffers.push_back(this->view_constants);
            res.constant_buffers.push_back(this->raycasting_constants);
            this->volume_technique = rendering_technique("raycasting", cs,
                std::move(res));
        }
    }

    // If the device or the viewport changed, invalidate the intermediate
    // buffers.
    if (contains_any(changed, factor_device, factor_viewport)) {
        this->ray_source = nullptr;
        this->ray_target = nullptr;
    }

    if (this->ray_target == nullptr) {
        assert(this->ray_source == nullptr);
        D3D11_TEXTURE2D_DESC desc;
        ATL::CComPtr<ID3D11Texture2D> tex;

        ::ZeroMemory(&desc, sizeof(desc));
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.Height = vp[1];
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.Width = vp[0];

        auto hr = dev->CreateTexture2D(&desc, nullptr, &tex);
        if (FAILED(hr)) {
            throw ATL::CAtlException(hr);
        }
        set_debug_object_name(tex.p, "ray_target_texture");

        hr = dev->CreateRenderTargetView(tex, nullptr, &this->ray_target);
        if (FAILED(hr)) {
            throw ATL::CAtlException(hr);
        }
        set_debug_object_name(this->ray_target.p, "ray_target_view");

        hr = dev->CreateShaderResourceView(tex, nullptr, &this->ray_source);
        if (FAILED(hr)) {
            throw ATL::CAtlException(hr);
        }
        set_debug_object_name(this->ray_target.p, "ray_source_view");
    }

    // Update the raycasting parameters.
    {
        // Update the raycasting parameters.
        ::ZeroMemory(&raycastingConstants, sizeof(raycastingConstants));
        raycastingConstants.BoxMax.x = bbe[0];
        raycastingConstants.BoxMax.y = bbe[1];
        raycastingConstants.BoxMax.z = bbe[2];
        raycastingConstants.BoxMax.w = 0.0f;
        raycastingConstants.BoxMin.x = bbs[0];
        raycastingConstants.BoxMin.y = bbs[1];
        raycastingConstants.BoxMin.z = bbs[2];
        raycastingConstants.BoxMin.w = 0.0f;
        raycastingConstants.ErtThreshold = config.get<float>(factor_ert_threshold);
        volume_benchmark_base::zero_is_max(raycastingConstants.ErtThreshold);
        raycastingConstants.MaxValue = 255.0f;  // TODO
        raycastingConstants.StepLimit = config.get<std::uint32_t>(factor_max_steps);
        volume_benchmark_base::zero_is_max(raycastingConstants.StepLimit);
        raycastingConstants.StepSize = this->calc_base_step_size()
            * config.get<float>(factor_step_size);
        raycastingConstants.ImageSize.x = vp[0];
        raycastingConstants.ImageSize.y = vp[1];

        ctx->UpdateSubresource(this->raycasting_constants.p, 0, nullptr,
            &raycastingConstants, 0, 0);
    }

    // Update the camera and view parameters.
    {
        ::ZeroMemory(&viewConstants, sizeof(viewConstants));
        const auto aspect = static_cast<float>(vp[0])
            / static_cast<float>(vp[1]);

        // First, set some basic camera parameters.
        this->camera.set_fovy(config.get<float>(factor_fovy_deg));
        this->camera.set_aspect_ratio(aspect);

        // Second, compute the current position based on the manoeuvre.
        apply_manoeuvre(this->camera, config, bbs, bbe);
        //auto hack = this->camera.get_look_from();
        //hack.z -= 50.0f;
        //this->camera.set_look_from(hack);

        // Once the camera is positioned, compute the clipping planes.
        auto clip = this->calc_clipping_planes(this->camera);
        clip.first = 0.1f;

        // Retrieve the final view parameters.
        DirectX::XMMATRIX vm, pm;
        {
            auto mat = DirectX::XMFLOAT4X4(glm::value_ptr(
                this->camera.get_projection_mx()));
            pm = DirectX::XMLoadFloat4x4(&mat);
        }
        {
            auto mat = DirectX::XMFLOAT4X4(glm::value_ptr(
                this->camera.get_view_mx()));
            vm = DirectX::XMLoadFloat4x4(&mat);
        }
        auto sm = DirectX::XMMatrixScaling(volSize[0], volSize[1], volSize[2]);
        vm = sm * vm;

        auto eye = DirectX::XMFLOAT4(300, 0, 0.5f * volSize[2] + 500.0f, 0);
        auto lookAt = DirectX::XMFLOAT4(0, 0, 0, 0);
        auto up = DirectX::XMFLOAT4(0, 1, 0, 0);
        auto view = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat4(&eye),
            DirectX::XMLoadFloat4(&lookAt), DirectX::XMLoadFloat4(&up));
        //auto mvp = sm * view * pm;
        auto mvp = sm * vm * pm;

        DirectX::XMStoreFloat4x4(&viewConstants.ViewProjMatrix,
            DirectX::XMMatrixTranspose(mvp));

        // Update the GPU resources.
        ctx->UpdateSubresource(this->view_constants.p, 0, nullptr,
            &viewConstants, 0, 0);
    }

    // Determine the number of thread groups to start.
    const auto groupX = static_cast<UINT>(ceil(static_cast<float>(vp[0])
        / 16.0f));
    const auto groupY = static_cast<UINT>(ceil(static_cast<float>(vp[1])
        / 16.0f));

    for (int i = 0; i < 10; ++i) {
        this->begin_ray_pass(ctx, vp);
        this->ray_technique.apply(ctx);
        ctx->DrawIndexed(36, 0, 0);

        this->begin_volume_pass(ctx);
        this->volume_technique.apply(ctx);
        ctx->Dispatch(groupX, groupY, 1);
        this->present_target();
        ::Sleep(120);
    }

    // TODO: implement the benchmark.

    return nullptr;
}


/*
 * trrojan::d3d11::two_pass_volume_benchmark::begin_ray_pass
 */
void trrojan::d3d11::two_pass_volume_benchmark::begin_ray_pass(
        ID3D11DeviceContext *ctx, const viewport_type& viewport) {
    assert(ctx != nullptr);
    static const float BLACK[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    D3D11_VIEWPORT vp;
    vp.TopLeftX = vp.TopLeftY = 0.0f;
    vp.Height = static_cast<float>(viewport[1]);
    vp.Width = static_cast<float>(viewport[0]);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    ctx->CSSetShaderResources(ray_slot, 1, &empty_srv);

    ctx->ClearRenderTargetView(this->ray_target, BLACK);
    ctx->OMSetRenderTargets(1, &this->ray_target.p, nullptr);
    ctx->RSSetViewports(1, &vp);
}


/*
 * trrojan::d3d11::two_pass_volume_benchmark::begin_volume_pass
 */
void trrojan::d3d11::two_pass_volume_benchmark::begin_volume_pass(
        ID3D11DeviceContext *ctx) {
    assert(ctx != nullptr);

    // Enable the UAV instead of the actual render target. This will also
    // ensure that the UAV is copied to the back buffer on present if a debug
    // target is enabled.
    auto target = this->switch_to_uav_target();
    if (target != nullptr) {
        this->volume_technique.set_uavs(target,
            static_cast<rendering_technique::shader_stages>(
            rendering_technique::shader_stage::compute));
    }

    // Make also sure that our ray render target is not bound any more.
    ctx->OMSetRenderTargets(1, &empty_rtv, nullptr);

    // Make sure that the most recent ray texture is bound.
    this->volume_technique.set_shader_resource_views(this->ray_source,
        static_cast<rendering_technique::shader_stages>(
        rendering_technique::shader_stage::compute), ray_slot);
}


/*
 * trrojan::d3d11::two_pass_volume_benchmark::empty_rtv
 */
ID3D11RenderTargetView *const
trrojan::d3d11::two_pass_volume_benchmark::empty_rtv = nullptr;


/*
 * trrojan::d3d11::two_pass_volume_benchmark::empty_srv
 */
ID3D11ShaderResourceView *const
trrojan::d3d11::two_pass_volume_benchmark::empty_srv = nullptr;
