/// <copyright file="debug_render_target.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <atlbase.h>
#include <Windows.h>

#include "trrojan/d3d11/render_target.h"


/* Forward declatations. */
struct DebugConstants;


namespace trrojan {
namespace d3d11 {

    /// <summary>
    /// The debug view is a render target using a visible window.
    /// </summary>
    class TRROJAND3D11_API debug_render_target : public render_target_base {

    public:

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        debug_render_target(void);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        virtual ~debug_render_target(void);

        /// <inheritdoc />
        virtual void present(void);

        /// <inheritdoc />
        virtual void resize(const unsigned int width,
            const unsigned int height);

    private:

        typedef render_target_base base;

        /// <summary>
        /// The name of the window class we are registering for the debug view.
        /// </summary>
        static const TCHAR *WINDOW_CLASS;

        /// <summary>
        /// The handler for window messages which makes the message pump exit if
        /// the escape key was pressed.
        /// </summary>
        static LRESULT WINAPI wndProc(HWND hWnd, UINT msg, WPARAM wParam,
            LPARAM lParam);

        /// <summary>
        /// Runs the message dispatcher.
        /// </summary>
        void doMsg(void);

        /// <summary>
        /// The handle of the debug window.
        /// </summary>
        std::atomic<HWND> hWnd;

        /// <summary>
        /// The message pumping thread.
        /// </summary>
        std::thread msgPump;

        /// <summary>
        /// The swap chain for the window.
        /// </summary>
        ATL::CComPtr<IDXGISwapChain> swapChain;
    };
}
}
