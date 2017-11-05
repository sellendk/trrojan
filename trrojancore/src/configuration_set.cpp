/// <copyright file="configuration_set.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>
/// <author>Valentin Bruder</author>

#include "trrojan/configuration_set.h"

#include <cassert>
#include <stdexcept>


/*
 * trrojan::configuration_set::add_factor
 */
void trrojan::configuration_set::add_factor(const factor& factor) {
    if (factor.name().empty()) {
        throw std::invalid_argument("The name of a factor must not be empty.");
    }
    if (factor.size() < 1) {
        throw std::invalid_argument("A factor must have at least one "
            "manifestation.");
    }
    if (this->contains_factor(factor.name())) {
        throw std::invalid_argument("The configuration set already contains a "
            "factor with the given name.");
    }

    this->_factors.push_back(factor);
}


/*
 * trrojan::configuration_set::replace_factor
 */
void trrojan::configuration_set::replace_factor(const factor& factor) {
    if (factor.size() < 1) {
        throw std::invalid_argument("A factor must have at least one "
            "manifestation.");
    }

    if (this->contains_factor(factor.name())) {
        assert(!factor.name().empty());
        auto it = this->findFactor(factor.name());
        assert(it != this->_factors.end());
        std::replace(this->_factors.begin(), this->_factors.end(), *it, factor);

    } else {
        this->add_factor(factor);
    }
}

/*
 * trrojan::configuration_set::find_factor
 */
const trrojan::factor *trrojan::configuration_set::find_factor(
        const std::string& name) const {
    auto it = this->findFactor(name);
    return (it != this->_factors.cend()) ? &(*it) : nullptr;
}


/*
 * trrojan::configuration_set::foreach_configuration
 */
bool trrojan::configuration_set::foreach_configuration(
        std::function<bool(const configuration&)> cb) const {
    bool retval = true;

    if (!this->_factors.empty() && cb) {
        size_t cntTests = 1;
        configuration config;
        std::vector<size_t> frequencies;

        config.reserve(this->_factors.size());
        for (auto& f : this->_factors) {
            frequencies.push_back(cntTests);
            cntTests *= f.size();
        }

        for (size_t i = 0; (i < cntTests) && retval; ++i) {
            config.clear();
            for (size_t j = 0; j < this->_factors.size(); ++j) {
                auto& f = this->_factors[j];
                auto ij = (i / frequencies[j]) % f.size();
                config.add(this->_factors[j].name(),
                    std::move(this->_factors[j][ij]));
            }
            retval = cb(config);
        }
    } /* end if (!this->factors.empty()) */

    return retval;
}


/*
 * trrojan::configuration_set::merge
 */
void trrojan::configuration_set::merge(const configuration_set& other,
        const bool overwrite) {
    for (auto& f : other._factors) {
        assert(f.size() > 0);
        auto it = this->findFactor(f.name());
        if (it != this->_factors.cend() && overwrite) {
            this->_factors.erase(it);
            this->_factors.push_back(f);
        } else if (it == this->_factors.cend()) {
            this->_factors.push_back(f);
        }
    }
}
