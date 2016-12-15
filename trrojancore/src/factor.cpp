/// <copyright file="factor.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/factor.h"

#include <cassert>
#include <stdexcept>


/*
 * trrojan::factor::operator []
 */
const trrojan::variant& trrojan::factor::operator [](const size_t i) const {
    if (this->impl == nullptr) {
        throw std::range_error("The factor does not contain the requested "
            "manifestation.");
    }

    return (*this->impl)[i];
}


/*
 * trrojan::factor::operator =
 */
trrojan::factor& trrojan::factor::operator =(factor&& rhs) {
    if (this != std::addressof(rhs)) {
        this->impl = std::move(impl);
    }
    return *this;
}


/*
 * trrojan::factor::operator ==
 */
bool trrojan::factor::operator ==(const factor& rhs) const {
    return (((this->impl == nullptr) && (rhs.impl == nullptr))
        || ((this->impl != nullptr) && (rhs.impl != nullptr)
        && (*this->impl == *rhs.impl)));
}
