/// <copyright file="plugin.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 - 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/d3d11/plugin.h"

#include "trrojan/d3d11/environment.h"
#include "trrojan/d3d11/sphere_benchmark.h"


/// <summary>
/// Handle of the plugin DLL.
/// </summary>
static HINSTANCE hTrrojanDll = NULL;


/// <summary>
/// Entry point of the DLL.
/// </summary>
BOOL WINAPI DllMain(HINSTANCE hDll, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hDll);
            ::hTrrojanDll = hDll;
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}


/// <summary>
/// Gets a new instance of the plugin descriptor.
/// </summary>
extern "C" TRROJAND3D11_API trrojan::plugin_base *get_trrojan_plugin(void) {
    return new trrojan::d3d11::plugin();
}


/*
 * trrojan::d3d11::plugin::load_resource
 */
std::vector<std::uint8_t> trrojan::d3d11::plugin::load_resource(LPCTSTR name,
        LPCSTR type) {
    auto hRes = ::FindResource(::hTrrojanDll, name, type);
    if (hRes == NULL) {
        std::error_code ec(::GetLastError(), std::system_category());
        throw std::system_error(ec, "Failed to find a resource.");
    }

    auto hGlobal = ::LoadResource(::hTrrojanDll, hRes);
    if (hGlobal == NULL) {
        std::error_code ec(::GetLastError(), std::system_category());
        throw std::system_error(ec, "Failed to load a resource.");
    }
    // Note: 'hGlobal' must not be freed. See
    // https://msdn.microsoft.com/de-de/library/windows/desktop/ms648046(v=vs.85).aspx

    auto hLock = ::LockResource(hGlobal);
    if (hLock == NULL) {
        std::error_code ec(::GetLastError(), std::system_category());
        throw std::system_error(ec, "Failed to lock a resource.");
    }

    auto retval = std::vector<std::uint8_t>(::SizeofResource(::hTrrojanDll,
        hRes));
    ::memcpy(retval.data(), hLock, retval.size());

    UnlockResource(hLock);

    return std::move(retval);
}


/*
 * trrojan::d3d11::plugin::~plugin
 */
trrojan::d3d11::plugin::~plugin(void) { }


/*
 * trrojan::d3d11::plugin::create_benchmarks
 */
size_t trrojan::d3d11::plugin::create_benchmarks(benchmark_list& dst) const {
    dst.emplace_back(std::make_shared<sphere_benchmark>());
    return 1;
}


/*
 * trrojan::d3d11::plugin::create_environments
 */
size_t trrojan::d3d11::plugin::create_environments(
        environment_list& dst) const {
    dst.emplace_back(std::make_shared<environment>());
    return 1;
}
