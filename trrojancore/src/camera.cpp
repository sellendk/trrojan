/// <copyright file="camera.cpp" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright © 2017 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Valentin Bruder</author>

#include "trrojan/camera.h"
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

trrojan::camera::camera(vec3 look_from, vec3 look_to, vec3 look_up, float near_plane,
                        float far_plane)
    : _look_from(look_from)
    , _look_to(look_to)
    , _look_up(look_up)
    , _near_plane_dist(near_plane)
    , _far_plane_dist(far_plane)
    , _invalid_view_mx(true)
    , _invalid_projection_mx(true)
{
}

const glm::mat4& trrojan::camera::get_view_mx() const
{
    if (_invalid_view_mx)
    {
        _view_mx = glm::lookAt(_look_from, _look_to, _look_up);
        _inverse_view_mx = glm::inverse(_view_mx);
        _invalid_view_mx = false;
    }
    return _view_mx;
}

const glm::mat4& trrojan::camera::get_projection_mx() const
{
    if (_invalid_projection_mx)
    {
        _projection_mx = calc_projection_mx();
        _inverse_projection_mx = glm::inverse(_projection_mx);
        _invalid_projection_mx = false;
    }
    return _projection_mx;
}

const glm::mat4& trrojan::camera::get_inverse_view_mx() const
{
    if (_invalid_view_mx) get_view_mx();
    return _inverse_view_mx;
}

const glm::mat4& trrojan::camera::get_inverse_projection_mx() const
{
    if (_invalid_projection_mx) get_projection_mx();
    return _inverse_projection_mx;
}

glm::vec3 trrojan::camera::get_world_pos_from_ndc(const vec3& ndc) const
{
    vec4 clip_coords = get_clip_pos_from_ndc(ndc);
    vec4 eye_coords = get_inverse_projection_mx() * clip_coords;
    vec4 world_coords = get_inverse_view_mx() * eye_coords;
    world_coords /= world_coords.w;
    return vec3(world_coords);
}

glm::vec4 trrojan::camera::get_clip_pos_from_ndc(const vec3& ndc) const
{
    const auto& projection = get_projection_mx();
    const float clipW = projection[2][3] / (ndc.z - (projection[2][2] / projection[3][2]));
    return vec4(ndc * clipW, clipW);
}

glm::vec3 trrojan::camera::get_ndc_from_normalized_screen_at_focus_point_depth(
    const vec2& normalized_screen_coords) const
{
    // Default to using focus point for depth
    vec4 look_to_clip_coords = get_projection_mx() * get_view_mx() * vec4(get_look_to(), 1.f);
    return vec3(2.f * normalized_screen_coords - 1.f, look_to_clip_coords.z / look_to_clip_coords.w);
}

glm::mat4 trrojan::camera::get_view_arcball(const glm::quat q, const float distance)
{
    glm::mat4 qmat = glm::mat4_cast(q);
    qmat = glm::translate(qmat, glm::vec3(0, 0, distance));
    return qmat;
}


glm::vec3 trrojan::camera::get_eye_ray(float x, float y)
{
    glm::vec4 tmp(x, y, 0.01f, 1.f);
    tmp = this->get_inverse_projection_mx() * tmp;
    tmp /= tmp.w;
    glm::vec3 ray = glm::vec3(tmp) - this->_look_from;
    return ray;
}

void trrojan::camera::rotate_fixed_to(const glm::quat q)
{
    this->set_look(_look_to + glm::rotate(q, _look_from - _look_to), _look_to,
                   glm::rotate(q, _look_up));
}

void trrojan::camera::rotate_fixed_from(const glm::quat q)
{
    this->set_look(_look_from, glm::rotate(q, this->_look_to - this->_look_from),
                   glm::rotate(q, _look_up));
}

/**
 * trrojan::camera::set_from_maneuver
 */
void trrojan::camera::set_from_maneuver(const std::string name, const glm::vec3 bbox_min,
                                        const glm::vec3 bbox_max, const int iteration,
                                        const float fovy, const int samples)
{
    if (iteration >= samples)
        throw std::invalid_argument("Iteration must be smaller than #samples.");

    // circle around axis
    if (name.find("circle") != std::string::npos)
    {
        // look at bounding box center
        this->set_look_to(bbox_min + (bbox_max - bbox_min)*0.5f);
        // fit view to bounding box x
        float bbox_length_x = bbox_max.x - bbox_min.x;
        float camera_dist = (bbox_length_x * 0.5f) / tan(fovy*0.5f);
        this->set_look_from(this->_look_to + glm::vec3(0, 0, camera_dist));
        // rotation
        float angle = (360.f / samples) * iteration;
        glm::vec3 axis(0.f);
        // create axis from x,y,z identifier
        if (name.find("_x") != std::string::npos) axis.x = 1;
        if (name.find("_y") != std::string::npos) axis.y = 1;
        if (name.find("_z") != std::string::npos) axis.z = 1;
        glm::quat q = glm::angleAxis(angle, glm::normalize(axis));
        this->rotate_fixed_to(q);
    }
    // fly-through on diagonal
    else if (name.find("diagonal") != std::string::npos)
    {
        glm::vec3 begin = bbox_min;
        glm::vec3 end = bbox_max;
        // point mirroring according to definition (e.g. "diagonal_zx" is between (0,1,1)->(1,0,0))
        if (name.find("_z") != std::string::npos)  std::swap(begin, end);        // back to front
        if (name.find("_x") != std::string::npos)  std::swap(begin.x, end.x);    // right to left
        if (name.find("_y") != std::string::npos)  std::swap(begin.y, end.y);    // top to bottom

        this->set_look_to(end);
        // NOTE: assuming uniform bounding box
        float camera_dist = (glm::length(end - begin) * 0.5f) / tan(fovy*0.5f);
        camera_dist *= (1.f - iteration/(float)samples);
        this->set_look_from(this->_look_to + (begin - end) * camera_dist);
    }
    else if (name.find("path") != std::string::npos)
    {
        // TODO: curves through volume (sine, cosine,...)
    }
    else if (name.find("radom") != std::string::npos)
    {
        this->set_look_to(glm::vec3(bbox_min + (bbox_max - bbox_min)*0.5f));
        // fit view to bounding box x
        float bbox_length_x = bbox_max.x - bbox_min.x;
        float camera_dist = (bbox_length_x * 0.5f) / tan(fovy*0.5f);
        // set random distance between center of bbox and 5x bbox fitted view
        this->set_look_from(this->_look_to + glm::vec3(0, 0, camera_dist * get_rand()*5.f));
        // rotate uniform sampled distance on sphere
        glm::quat q = {get_rand(), get_rand(), get_rand(), get_rand()};
        q = glm::normalize(q);
        this->rotate_fixed_to(q);
    }
}

// Perspective camera
trrojan::perspective_camera::perspective_camera(vec3 look_from, vec3 look_to, vec3 look_up,
                                                float near_plane, float far_plane,
                                                float field_of_view, float aspect_ratio)
    : camera(look_from, look_to, look_up, near_plane, far_plane)
    , _fovy(field_of_view)
    , _aspect_ratio(aspect_ratio)
{
}

void trrojan::perspective_camera::set_from_maneuver(const std::string name, const glm::vec3 bbox_min,
                                                    const glm::vec3 bbox_max, const int iteration,
                                                    const int samples)
{
    camera::set_from_maneuver(name, bbox_min, bbox_max, iteration, get_fovy(), samples);
}

// Orthographic camera
trrojan::orthographic_camera::orthographic_camera(vec3 look_from, vec3 look_to, vec3 look_up,
                                                  float near_plane, float far_plane, vec4 frustum)
    : camera(look_from, look_to, look_up, near_plane, far_plane), _frustum(frustum)
{
}

float trrojan::orthographic_camera::get_aspect_ratio() const
{
    // Left, right, bottom, top view volume
    const float width{_frustum.y - _frustum.x};
    const float height{_frustum.w - _frustum.z};
    return width / height;
}

void trrojan::orthographic_camera::set_aspect_ratio(float val)
{
    // Left, right, bottom, top view volume
    const float width{_frustum.y - _frustum.x};
    const float height{width / val};
    _frustum.z = -height / 2.0f;
    _frustum.w = +height / 2.0f;
    invalidate_projection_mx();
}

void trrojan::orthographic_camera::set_from_maneuver(const std::string name,
                                                     const glm::vec3 bbox_min,
                                                     const glm::vec3 bbox_max, const int iteration,
                                                     const int samples)
{
    // TODO: handle fovy
    camera::set_from_maneuver(name, bbox_min, bbox_max, iteration, 90.f, samples);
}
