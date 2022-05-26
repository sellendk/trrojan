// <copyright file="d2d_overlay.h" company="Visualisierungsinstitut der Universit�t Stuttgart">
// Copyright � 2022 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
// Licensed under the MIT licence. See LICENCE.txt file in the project root for full licence information.
// </copyright>
// <author>Christoph M�ller</author>

#pragma once

#include <cassert>
#include <memory>

#include <Windows.h>
#include <atlbase.h>
#include <d2d1.h>
#include <d3d11.h>
#include <dwrite.h>

#include "trrojan/device.h"

#include "trrojan/d3d11/export.h"


namespace trrojan {
namespace d3d11 {

    /// <summary>
    /// A D2D render target based on a D3D11 texture that can be used to
    /// produce a 2D overlay.
    /// </summary>
    /// <remarks>
    /// <para>There are two ways of using this class: The first one is
    /// instantiating it and calling either the convenience methods for
    /// creating resources and drawing text or issuing custom draw
    /// commands via the render target exposed by the overlay.</para>
    /// <para>The second way is deriving a custom overlay from this
    /// class. This method has the advantage that overriding the two
    /// methods <see cref="on_resize" /> and <see cref="on_resized" />
    /// facilitates the life-time management for Direct2D and DirectWrite
    /// resources.</para>
    /// </remarks>
    class TRROJAND3D11_API d2d_overlay {

    public:

        /// <summary>
        /// Gets the font used in the given text format.
        /// </summary>
        /// <remarks>
        /// This information might be valuable to compute positions and sizes
        /// depending on the font metrics.
        /// </remarks>
        /// <param name="format"></param>
        /// <returns></returns>
        static ATL::CComPtr<IDWriteFont> get_font(IDWriteTextFormat *format);

        /// <summary>
        /// Initialises a new isntance using the specified texture as render
        /// target.
        /// </summary>
        /// <param name="texture"></param>
        d2d_overlay(ID3D11Texture2D *texture);

        /// <summary>
        /// Initialises an overlay for the given swap chain.
        /// </summary>
        /// <param name="device"></param>
        /// <param name="swap_chain"></param>
        d2d_overlay(ID3D11Device *device, IDXGISwapChain *swap_chain);

        d2d_overlay(const d2d_overlay& rhs) = delete;

        virtual ~d2d_overlay(void) = default;

        /// <summary>
        /// Begin drawing 2D content.
        /// </summary>
        inline void begin_draw(void) {
            assert(this->_d2d_target != nullptr);
            this->_d2d_target->BeginDraw();
        }

        /// <summary>
        /// Clears the 2D content.
        /// </summary>
        /// <param name=""></param>
        inline void clear(void) {
            assert(this->_d2d_target != nullptr);
            this->_d2d_target->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        }

        /// <summary>
        /// Create a solid brush of the given colour.
        /// </summary>
        /// <param name="colour"></param>
        /// <returns></returns>
        ATL::CComPtr<ID2D1Brush> create_brush(const D2D1::ColorF& colour);

        /// <summary>
        /// Creates a text format.
        /// </summary>
        /// <param name="font_family"></param>
        /// <param name="font_size"></param>
        /// <param name="font_weight"></param>
        /// <param name="font_style"></param>
        /// <param name="font_stretch"></param>
        /// <param name="locale_name"></param>
        /// <returns></returns>
        ATL::CComPtr<IDWriteTextFormat> create_text_format(
            const wchar_t *font_family, const float font_size,
            const DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_NORMAL,
            const DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL,
            const DWRITE_FONT_STRETCH font_stretch = DWRITE_FONT_STRETCH_NORMAL,
            const wchar_t *locale_name = nullptr);

        /// <summary>
        /// Draws the overlay on the currently active render target.
        /// </summary>
        void draw(void);

        /// <summary>
        /// Draws text on the overlay.
        /// </summary>
        /// <param name="text"></param>
        /// <param name="format"></param>
        /// <param name="brush"></param>
        /// <param name="layout_rect"></param>
        void draw_text(const wchar_t *text, IDWriteTextFormat *format,
            ID2D1Brush *brush, const D2D1_RECT_F *layout_rect = nullptr);

        /// <summary>
        /// Daws a text using a transient <see cref="IDWriteTextFormat" /> for
        /// formatting.
        /// </summary>
        /// <remarks>
        /// This method is not recommended for production use. Allocate and
        /// reuse graphics resources as long as the render target has not been
        /// invalidated.
        /// </remarks>
        /// <param name="text"></param>
        /// <param name="font_family"></param>
        /// <param name="font_size"></param>
        /// <param name="brush"></param>
        /// <param name="font_weight"></param>
        /// <param name="font_style"></param>
        /// <param name="font_stretch"></param>
        /// <param name="layout_rect"></param>
        /// <param name="locale_name"></param>
        inline void draw_text(const wchar_t *text, const wchar_t *font_family,
                const float font_size, ID2D1Brush *brush,
                const DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_NORMAL,
                const DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL,
                const DWRITE_FONT_STRETCH font_stretch = DWRITE_FONT_STRETCH_NORMAL,
                const D2D1_RECT_F *layout_rect = nullptr,
                const wchar_t *locale_name = nullptr) {
            auto format = this->create_text_format(font_family, font_size,
                font_weight, font_style, font_stretch, locale_name);
            this->draw_text(text, format, brush, layout_rect);
        }

        /// <summary>
        /// Draws a text using a transient <see cref="IDWriteTextFormat" /> and
        /// <see cref="ID2D1Brush" /> for rendering.
        /// </summary>
        /// <remarks>
        /// This method is not recommended for production use. Allocate and
        /// reuse graphics resources as long as the render target has not been
        /// invalidated.
        /// </remarks>
        /// <param name="text"></param>
        /// <param name="font_family"></param>
        /// <param name="font_size"></param>
        /// <param name="colour"></param>
        /// <param name="font_weight"></param>
        /// <param name="font_style"></param>
        /// <param name="font_stretch"></param>
        /// <param name="layout_rect"></param>
        /// <param name="locale_name"></param>
        inline void draw_text(const wchar_t *text, const wchar_t *font_family,
                const float font_size, const D2D1::ColorF colour,
                const DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_NORMAL,
                const DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL,
                const DWRITE_FONT_STRETCH font_stretch = DWRITE_FONT_STRETCH_NORMAL,
                const D2D1_RECT_F *layout_rect = nullptr,
                const wchar_t *locale_name = nullptr) {
            auto format = this->create_text_format(font_family, font_size,
                font_weight, font_style, font_stretch, locale_name);
            auto brush = this->create_brush(colour);
            this->draw_text(text, format, brush, layout_rect);
        }

        /// <summary>
        /// End drawing 2D content.
        /// </summary>
        inline void end_draw(void) {
            assert(this->_d2d_target != nullptr);
            this->_d2d_target->EndDraw();
        }

        /// <summary>
        /// Gets the Direct2D factory used to create the overlay.
        /// </summary>
        /// <returns></returns>
        inline ATL::CComPtr<ID2D1Factory> get_d2d_factory(void) const {
            return this->_d2d_factory;
        }

        /// <summary>
        /// Gets the DirectWrite factory used to render text on the overlay.
        /// </summary>
        /// <returns></returns>
        inline ATL::CComPtr<IDWriteFactory> get_dwrite_factory(void) const {
            return this->_dwrite_factory;
        }

        /// <summary>
        /// Resize the render target to the specified dimensions.
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        void resize(const UINT width, const UINT height);

        /// <summary>
        /// Sets the 2D transformation to be the identity matrix.
        /// </summary>
        void set_identity_transform(void) {
            assert(this->_d2d_target != nullptr);
            this->_d2d_target->SetTransform(D2D1::IdentityMatrix());
        }

        d2d_overlay& operator =(const d2d_overlay& rhs) = delete;

        /// <summary>
        /// Gets the render target of the overlay.
        /// </summary>
        inline operator ATL::CComPtr<ID2D1RenderTarget>(void) {
            return this->_d2d_target;
        }

    protected:

        /// <summary>
        /// Subclasses must release all resources obtained from the render
        /// target in this method, which is called before the render target
        /// is recreated for resizing.
        /// </summary>
        virtual void on_resize(void);

        /// <summary>
        /// Subclasses can allocate resources obtained from the render target.
        /// </summary>
        /// <remarks>
        /// This method will be called on creation of and after resizing the
        /// render target.
        /// </remarks>
        virtual void on_resized(void);

    private:

        void create_target_dependent_resources(IDXGISurface *surface);

        void create_target_dependent_resources(void);

        void create_target_independent_resources(void);

        void release_target_dependent_resources(void);

        ATL::CComPtr<ID3D11BlendState> _blend_state;
        ATL::CComPtr<ID2D1Factory> _d2d_factory;
        ATL::CComPtr<ID2D1RenderTarget> _d2d_target;
        ATL::CComPtr<ID3D11Device> _d3d_device;
        ATL::CComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        ATL::CComPtr<IDWriteFactory> _dwrite_factory;
        ATL::CComPtr<ID3D11InputLayout> _input_layout;
        ATL::CComPtr<ID3D11SamplerState> _sampler;
        ATL::CComPtr<ID3D11PixelShader> _ps;
        ATL::CComPtr<ID3D11RasterizerState> _rasteriser_state;
        ATL::CComPtr<ID3D11ShaderResourceView> _srv;
        ATL::CComPtr<ID3D11Texture2D> _texture;
        ATL::CComPtr<ID3D11VertexShader> _vs;
    };
}
}
