/// <copyright file="random_sphere_data_set.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2017 - 2018 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/d3d11/random_sphere_data_set.h"

#include <cassert>
#include <cinttypes>
#include <random>

#include "trrojan/log.h"
#include "trrojan/text.h"


/*
 * trrojan::d3d11::random_sphere_data_set::create
 */
trrojan::d3d11::sphere_data_set
trrojan::d3d11::random_sphere_data_set::create(ID3D11Device *device,
        const create_flags flags,
        const sphere_type sphereType,
        const size_type cntParticles,
        const std::array<float, 3>& domainSize,
        const std::array<float, 2>& sphereSize,
        const std::uint32_t seed) {
    static const create_flags VALID_INPUT_FLAGS // Flags directly copied from user input.
        = sphere_data_set_base::property_structured_resource;
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA id;
    std::uniform_real_distribution<float> posDist(0, 1);
    std::uniform_real_distribution<float> radDist(sphereSize[0], sphereSize[1]);
    std::vector<std::uint8_t> particles;
    std::mt19937 prng;
    std::shared_ptr<random_sphere_data_set> retval(
        new random_sphere_data_set(sphereType));
    assert(retval->_properties == 0);

    if (device == nullptr) {
        throw std::invalid_argument("The Direct3D device to create the vertex "
            "buffer on must not be nullptr.");
    }

    prng.seed(seed);
    particles.resize(cntParticles * retval->stride());

    log::instance().write_line(log_level::verbose, "Creating %u random "
        "sphere(s) of type %u on a domain of [%f, %f, %f] with a uniformly "
        "distributed size in [%f, %f]. The random seed is %u.", cntParticles,
        static_cast<std::uint32_t>(sphereType), domainSize[0], domainSize[1],
        domainSize[2], sphereSize[0], sphereSize[1], seed);

    retval->_bbox[0][0] = retval->_bbox[0][1] = retval->_bbox[0][2]
        = (std::numeric_limits<float>::max)();
    retval->_bbox[1][0] = retval->_bbox[1][1] = retval->_bbox[1][2]
        = (std::numeric_limits<float>::min)();
    retval->_max_radius
        = (std::numeric_limits<decltype(retval->_max_radius)>::min)();
    for (std::uint32_t i = 0; i < cntParticles; ++i) {
        auto p = particles.data() + (i * retval->stride());
        auto g = static_cast<float>(i) / static_cast<float>(cntParticles);
        auto pos = reinterpret_cast<DirectX::XMFLOAT4 *>(p);

        pos->x = posDist(prng) * domainSize[0] - 0.5f * domainSize[0];
        pos->y = posDist(prng) * domainSize[1] - 0.5f * domainSize[1];
        pos->z = posDist(prng) * domainSize[2] - 0.5f * domainSize[2];
        minmax(retval->_bbox[0], retval->_bbox[1], *pos);

        switch (sphereType) {
            case sphere_type::pos_rad_intensity:
            case sphere_type::pos_rad_rgba32:
            case sphere_type::pos_rad_rgba8:
                pos->w = radDist(prng);
                if (retval->_max_radius < pos->w) {
                    retval->_max_radius = pos->w;
                }
                break;

            default:
                pos->w = 0.0f;
        }

        switch (sphereType) {
            case sphere_type::pos_intensity:
            case sphere_type::pos_rad_intensity:
                *reinterpret_cast<float *>(pos + 1) = g;
                break;

            case sphere_type::pos_rgba32:
            case sphere_type::pos_rad_rgba32:
                *reinterpret_cast<float *>(pos + 1) = g;
                *reinterpret_cast<float *>(pos + 2) = g;
                *reinterpret_cast<float *>(pos + 3) = g;
                *reinterpret_cast<float *>(pos + 4) = 1.0f;
                break;

            case sphere_type::pos_rgba8:
            case sphere_type::pos_rad_rgba8:
                *reinterpret_cast<COLORREF *>(pos + 1)
                    = RGB(g * 255, g * 255, g * 255);
                break;

            default:
                throw std::runtime_error("Unexpected sphere format.");
        }
    }

    ::ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = static_cast<UINT>(particles.size());
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.CPUAccessFlags = 0;
    if ((flags & property_structured_resource) != 0) {
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.StructureByteStride = retval->stride();
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    } else {
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    }

    ::ZeroMemory(&id, sizeof(id));
    id.pSysMem = particles.data();

    {
        auto hr = device->CreateBuffer(&bufferDesc, &id, &retval->_buffer);
        if (FAILED(hr)) {
            throw ATL::CAtlException(hr);
        }
    }

    retval->_size = cntParticles;
    retval->_layout = random_sphere_data_set::get_input_layout(sphereType);

    retval->_properties |= (flags & VALID_INPUT_FLAGS);
    retval->_properties |= random_sphere_data_set::get_properties(sphereType);

    return retval;
}


/*
 * trrojan::d3d11::random_sphere_data_set::create
 */
trrojan::d3d11::sphere_data_set
trrojan::d3d11::random_sphere_data_set::create(ID3D11Device *device,
        const create_flags flags, const std::string& configuration) {
    static const std::runtime_error PARSE_ERROR("The configuration description "
        "of the random spheres is invalid. The configuration must have the "
        "following format: \"<sphere type> : <number of spheres> : <random "
        "seed or \"-\"> : <domain size> : <sphere size range>\"");
    static const char SEPARATOR = ':';
#define _ADD_SPHERE_TYPE(n) { #n, sphere_type::n }
    static const struct {
        const char *name;
        sphere_type type;
    } SPHERE_TYPES[] = {
        _ADD_SPHERE_TYPE(pos_intensity),
        _ADD_SPHERE_TYPE(pos_rgba32),
        _ADD_SPHERE_TYPE(pos_rgba8),
        _ADD_SPHERE_TYPE(pos_rad_intensity),
        _ADD_SPHERE_TYPE(pos_rad_rgba32),
        _ADD_SPHERE_TYPE(pos_rad_rgba8)
    };
#undef _ADD_SPHERE_TYPE

    std::uint32_t cntParticles = 0;
    std::array<float, 3> domainSize = { 0, 0, 0 };
    std::uint32_t seed = 0;
    std::array<float, 2> sphereSize = { 0 ,0 };
    sphere_type sphereType = unspecified;

    /* Parse the type of spheres to generate. */
    auto tokBegin = configuration.begin();
    auto tokEnd = std::find(configuration.begin(), configuration.end(),
        SEPARATOR);
    if (tokEnd == configuration.end()) {
        throw PARSE_ERROR;
    }

    auto token = tolower(trim(std::string(tokBegin, tokEnd)));
    for (auto& t : SPHERE_TYPES) {
        if (token == t.name) {
            sphereType = t.type;
            break;
        }
    }
    if (sphereType == sphere_type::unspecified) {
        throw std::runtime_error("The type of random spheres to generate is "
            "invalid.");
    }
    if ((flags & property_float_colour) != 0) {
        // Force 8-bit colours to float on request.
        switch (sphereType) {
            case sphere_type::pos_rgba8:
                sphereType = sphere_type::pos_rgba32;
                break;

            case sphere_type::pos_rad_rgba8:
                sphereType = sphere_type::pos_rad_rgba32;
                break;
        }
    }

    /* Parse the number of spheres to generate. */
    tokBegin = ++tokEnd;
    tokEnd = std::find(tokEnd, configuration.end(), SEPARATOR);
    if (tokEnd == configuration.end()) {
        throw PARSE_ERROR;
    }

    cntParticles = parse<decltype(cntParticles)>(std::string(tokBegin, tokEnd));

    /* Parse the random seed. */
    tokBegin = ++tokEnd;
    tokEnd = std::find(tokEnd, configuration.end(), SEPARATOR);
    if (tokEnd == configuration.end()) {
        throw PARSE_ERROR;
    }

    try {
        seed = parse<decltype(seed)>(std::string(tokBegin, tokEnd));
    } catch (...) {
        // Special case: use "real" random seed.
        std::random_device rnd;
        seed = rnd();
    }

    /* Parse the size of the domain. */
    tokBegin = ++tokEnd;
    tokEnd = std::find(tokEnd, configuration.end(), SEPARATOR);
    if (tokEnd == configuration.end()) {
        throw PARSE_ERROR;
    }

    domainSize = parse<decltype(domainSize)>(std::string(tokBegin, tokEnd));

    /* Parse the range of possible sphere sizes. */
    tokBegin = ++tokEnd;
    tokEnd = configuration.end();
    sphereSize = parse<decltype(sphereSize)>(std::string(tokBegin, tokEnd));

    return random_sphere_data_set::create(device, flags, sphereType,
        cntParticles, domainSize, sphereSize, seed);
}


/*
 * trrojan::d3d11::random_sphere_data_set::get_input_layout
 */
std::vector<D3D11_INPUT_ELEMENT_DESC>
trrojan::d3d11::random_sphere_data_set::get_input_layout(
        const sphere_type type) {
    static const std::vector<D3D11_INPUT_ELEMENT_DESC> _INT = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    static const std::vector<D3D11_INPUT_ELEMENT_DESC> _RGBA8 = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    static const std::vector<D3D11_INPUT_ELEMENT_DESC> _RGBA32 = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };


    switch (type) {
        case sphere_type::pos_intensity:
        case sphere_type::pos_rad_intensity:
            return _INT;

        case sphere_type::pos_rgba8:
        case sphere_type::pos_rad_rgba8:
            return _RGBA8;

        case sphere_type::pos_rgba32:
        case sphere_type::pos_rad_rgba32:
            return _RGBA32;

        default:
            throw std::runtime_error("Unexpected sphere format.");
    }
}


/*
 * trrojan::d3d11::random_sphere_data_set::get_properties
 */
trrojan::d3d11::sphere_data_set_base::properties_type
trrojan::d3d11::random_sphere_data_set::get_properties(const sphere_type type) {
    properties_type retval = 0;

    switch (type) {
        case pos_rad_intensity:
        case pos_rad_rgba32:
        case pos_rad_rgba8:
            retval |= property_per_sphere_radius;
            break;
    }

    switch (type) {
        case pos_rgba32:
        case pos_rad_rgba32:
            retval |= property_float_colour;
            /* falls through. */
        case pos_rgba8:
        case pos_rad_rgba8:
            retval |= property_per_sphere_colour;
            break;
    }

    switch (type) {
        case pos_intensity:
        case pos_rad_intensity:
            retval |= property_per_sphere_intensity;
            break;
    }

    return retval;
}


/*
 * trrojan::d3d11::random_sphere_data_set::get_stride
 */
trrojan::d3d11::random_sphere_data_set::size_type
trrojan::d3d11::random_sphere_data_set::get_stride(
        const sphere_type type) {
    switch (type) {
        case sphere_type::pos_intensity:
        case sphere_type::pos_rad_intensity:
            return sizeof(random_sphere_intensity);

        case sphere_type::pos_rgba8:
        case sphere_type::pos_rad_rgba8:
            return sizeof(random_sphere_rgba8);

        case sphere_type::pos_rgba32:
        case sphere_type::pos_rad_rgba32:
            return sizeof(random_sphere_rgba32);

        default:
            throw std::runtime_error("Unexpected sphere format.");
    }
}


/*
 * trrojan::d3d11::random_sphere_data_set::bounding_box
 */
void trrojan::d3d11::random_sphere_data_set::bounding_box(point_type& outMin,
        point_type& outMax) const {
    outMin = this->_bbox[0];
    outMax = this->_bbox[1];
}


/*
 * trrojan::d3d11::random_sphere_data_set::max_radius
 */
float trrojan::d3d11::random_sphere_data_set::max_radius(void) const {
    return this->_max_radius;
}


/*
 * trrojan::d3d11::random_sphere_data_set::size
 */
trrojan::d3d11::random_sphere_data_set::size_type
trrojan::d3d11::random_sphere_data_set::size(void) const {
    return this->_size;
}


/*
 * trrojan::d3d11::random_sphere_data_set::stride
 */
trrojan::d3d11::random_sphere_data_set::size_type
trrojan::d3d11::random_sphere_data_set::stride(void) const {
    return random_sphere_data_set::get_stride(this->_type);
}


/*
 * trrojan::d3d11::random_sphere_data_set::minmax
 */
void trrojan::d3d11::random_sphere_data_set::minmax(point_type& i, point_type& a,
        const DirectX::XMFLOAT4& v) {
    if (v.x < i[0]) {
        i[0] = v.x;
    }
    if (v.y < i[1]) {
        i[1] = v.y;
    }
    if (v.z < i[2]) {
        i[2] = v.z;
    }

    if (v.x > a[0]) {
        a[0] = v.x;
    }
    if (v.y > a[1]) {
        a[1] = v.y;
    }
    if (v.z > a[2]) {
        a[2] = v.z;
    }
}


/*
 * trrojan::d3d11::random_sphere_data_set::random_sphere_data_set
 */
trrojan::d3d11::random_sphere_data_set::random_sphere_data_set(
        const sphere_type type) : _max_radius(0.0f), _size(0), _type(type) {
    ::memset(this->_bbox, 0, sizeof(this->_bbox));
}
