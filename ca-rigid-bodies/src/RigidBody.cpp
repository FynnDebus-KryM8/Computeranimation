//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "RigidBody.h"

//== IMPLEMENTATION ==========================================================

RigidBody::RigidBody(const std::vector<vec2>& _points, float _mass,
                     const vec2 linVelocity)
    : color(0, 0, 1)
{
    // copy points and mass
    points = _points;
    mass = _mass;

    // compute center of mass
    position[0] = position[1] = 0.0;
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        position += points[i];
    }
    position /= (float)points.size();

    // compute bounding sphere radius
    radius = 0;
    for (auto p : points)
    {
        float dist = norm(p - position);
        if (dist > radius)
        {
            radius = dist;
        }
    }

    // compute points in local coordinate system
    r.resize(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        r[i] = points[i] - position;
    }

    // compute moment of inertia
    inertia = 0.0;
    float mi = mass / (float)points.size();
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        inertia += mi * norm(r[i]) * norm(r[i]);
    }

    // initialize rotation
    orientation = 0.0;

    // initialize velocities & forces
    linear_velocity = linVelocity;
    force = vec2(0.0, 0.0);
    angular_velocity = torque = 0.0;
}

//-----------------------------------------------------------------------------

void RigidBody::update_points()
{
    const float s = sin(orientation), c = cos(orientation);

    for (unsigned int i = 0; i < points.size(); ++i)
    {
        points[i] = position +
                    vec2(c * r[i][0] - s * r[i][1], s * r[i][0] + c * r[i][1]);
    }
}

//=============================================================================
