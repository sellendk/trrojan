/// <copyright file="mmpld_base.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <istream>
#include <vector>

#include <atlbase.h>
#include <Windows.h>
#include <d3d11.h>

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

    };

}
}
