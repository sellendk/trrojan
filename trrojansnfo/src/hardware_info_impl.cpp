/// <copyright file="hardware_info_impl.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2018 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
/// Copyright � 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "hardware_info_impl.h"

#include <algorithm>
#include <iostream>
#include <regex>

#include "setupapiutil.h"
#include "utilities.h"

// $  udevadm info -a -n /dev/video11
// lspci -k | grep -EA3 'VGA|3D|Display'


/*
 * trrojan::sysinfo::detail::hardware_info_impl::update
 */
void trrojan::sysinfo::detail::hardware_info_impl::update(void) {
#if defined(_WIN32)
    /* Enumerate the GPUs.*/
    {
        this->gpus.clear();

        auto classID = GUID_DEVCLASS_DISPLAY;
        detail::enum_class_devices(&classID, [this](HDEVINFO hDev,
                SP_DEVINFO_DATA& data) {
            this->gpus.emplace_back(device_info_impl(hDev, data));
            return true;
        }, DIGCF_PRESENT | DIGCF_PROFILE);
    }
#else /* defined(_WIN32) */
    // TODO: Implement
#endif /* defined(_WIN32) */
}

