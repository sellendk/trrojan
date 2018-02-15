/// <copyright file="mmpld_base.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <utility>

#include <atlbase.h>
#include <Windows.h>
#include <d3d11.h>

#include "trrojan/camera.h"
#include "trrojan/graphics_benchmark_base.h"
#include "trrojan/log.h"
#include "trrojan/mmpld_reader.h"

#include "trrojan/d3d11/export.h"
#include "trrojan/d3d11/device.h"


namespace trrojan {
namespace d3d11 {

    /// <summary>
    /// Base class for using MMPLD files.
    /// </summary>
    class TRROJAND3D11_API mmpld_base {

    public:

        virtual ~mmpld_base(void) = default;

    protected:

        /// <summary>
        /// The type of the colour enumeration.
        /// </summary>
        typedef mmpld_reader::colour_type mmpld_colour_type;

        /// <summary>
        /// The bitmask describing the input properties.
        /// </summary>
        typedef std::underlying_type<mmpld_reader::shader_properties>::type
            mmpld_input_properties;

        /// <summary>
        /// the type of the vertex type enumeration.
        /// </summary>
        typedef mmpld_reader::vertex_type mmpld_vertex_type;

        /// <summary>
        /// Creates a vertex input layout descriptor for the given MMPLD list
        /// header.
        /// </summary>
        /// <param name="header">The header of the particle list to get the
        /// vertex format for.</param>
        /// <returns>The vertex descriptor.</returns>
        static std::vector<D3D11_INPUT_ELEMENT_DESC> get_mmpld_layout(
            const mmpld_reader::list_header& header);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        mmpld_base(void);

        /// <summary>
        /// Gets the bounding box of the current <see cref="mmpld_list" />.
        /// </summary>
        void get_mmpld_bounding_box(graphics_benchmark_base::point_type& outMin,
            graphics_benchmark_base::point_type& outMax) const;

        /// <summary>
        /// Gets the centre point of the current <see cref="mmpld_list" />.
        /// </summary>
        graphics_benchmark_base::point_type get_mmpld_centre(void) const;

        /// <summary>
        /// Make a suggestion for near and far clipping plane for the current
        /// <see cref="mmpld_list" /> and the given
        /// <see cref="trrojan::camera" />.
        /// </summary>
        std::pair<float, float> get_mmpld_clipping(const camera& cam) const;

        /// <summary>
        /// Determine the input features of the <see cref="mmpld_list" />.
        /// </summary>
        /// <param name="perPixelXfer">If <c>true</c>, assume transfer function
        ///being applied in the pixel shader (<c>SPHERE_INPUT_PP_INTENSITY</c>
        /// instead of <c>SPHERE_INPUT_PV_INTENSITY</c>).</param>
        /// <returns></returns>
        mmpld_input_properties get_mmpld_input_properties(
            const bool perPixelXfer) const;

        /// <summary>
        /// Gets the 3D size the current <see cref="mmpld_list" /> (x-axis,
        /// y-axis and z-axis).
        /// </summary>
        std::array<float, 3> get_mmpld_size(void) const;

        /// <summary>
        /// Opens the MMPLD file at the specified location and reads its header
        /// and seek table.
        /// </summary>
        /// <param name="path"></param>
        /// <returns>The state of the stream after the above-mentioned
        /// operations have been performed.</returns>
        bool open_mmpld(const char *path);

        /// <summary>
        /// Read the given frame from the previously opened MMPLD stream and
        /// upload it to a vertex buffer on the given device.
        /// </summary>
        /// <param name="device"></param>
        /// <param name="frame"></param>
        /// <returns></returns>
        ATL::CComPtr<ID3D11Buffer> read_mmpld_frame(ID3D11Device *device,
            const unsigned int frame);

        /// <summary>
        /// The header of the currently opened MMPLD file.
        /// </summary>
        /// <remarks>
        /// This variable is only valid while <see cref="mmpld_stream" /> is
        /// good. Otherwise, its content might be bogus.
        /// </remarks>
        mmpld_reader::file_header mmpld_header;

        /// <summary>
        /// The input layout of the last read MMPLD frame particle list.
        /// </summary>
        std::vector<D3D11_INPUT_ELEMENT_DESC> mmpld_layout;

        /// <summary>
        /// The list header of the last read MMPLD particle list.
        /// </summary>
        mmpld_reader::list_header mmpld_list;

        /// <summary>
        /// The stream of the currently opened MMPLD file.
        /// </summary>
        std::ifstream mmpld_stream;

        /// <summary>
        /// The seek table for the currently opened MMPLD file.
        /// </summary>
        /// <remarks>
        /// This variable is only valid while <see cref="mmpld_stream" /> is
        /// good. Otherwise, its content might be bogus.
        /// </remarks>
        mmpld_reader::seek_table mmpld_seek_table;

    private:

        /// <summary>
        /// Radius used for clipping computation of the current particle list.
        /// </summary>
        float mmpld_max_radius;
    };

} /* end namespace d3d11 */
} /* end namespace trrojan */
