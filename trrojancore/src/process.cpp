/// <copyright file="process.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/process.h"

#include <cstdio>
#include <cstdlib>
#include <system_error>
#include <vector>

#ifndef _WIN32
#include <sys/types.h>
#include <errno.h>
#endif /* _WIN32 */


/*
 * trrojan::get_module_file_name
 */
std::string TRROJANCORE_API trrojan::get_module_file_name(void) {
    typedef decltype(get_module_file_name()) string_type;
    std::vector<string_type::value_type> retval;

#ifdef _WIN32
    DWORD error = ERROR_SUCCESS;
    DWORD bufLen = MAX_PATH + 1;
    DWORD strLen = 0;

    do {
        retval.resize(bufLen);
        strLen = ::GetModuleFileNameA(NULL, retval.data(), bufLen);
        error = ::GetLastError();
        if (strLen == 0) {
            std::error_code ec(::GetLastError(), std::system_category());
            throw std::system_error(ec, "GetModuleFileName failed.");
        }
        bufLen *= 2;
    } while (error == ERROR_INSUFFICIENT_BUFFER);

    return std::move(string_type(retval.data(), strLen));

#else /* _WIN32 */
    retval.resize(512);
    ssizet_t size = 0;

    do {
        size = ::readlink("/proc/self/exe", retval.data(), retval.size());
        if (size == -1) {
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, "GetModuleFileName failed.");
        }
        retval.resize(retval.size() * 2);
    } while (retval.size() <= size);

    return std::move(string_type(retval.data(), size));
#endif /* _WIN32 */
}


/*
 * trrojan::get_process_id
 */
trrojan::process_id TRROJANCORE_API trrojan::get_process_id(void) {
#ifdef _WIN32
    return ::GetCurrentProcessId();
#else /* _WIN32 */
    return ::getpid();
#endif /* _WIN32 */
}
