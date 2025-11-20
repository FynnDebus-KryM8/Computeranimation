//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================
#pragma once
//=============================================================================

#include <vector>
#include <pmp/MatVec.h>
using namespace pmp;

//== CLASS DEFINITION =========================================================

/** \class Rigid_body Rigid_body.h
 Class for representing a rigid body.
 It represents the rigid body by the position of its center of gravity
 and its orientation (rotation angle). For these two components it also
 stores velocities, forces, and mass/inertia.
 */
class RigidBody
{
public:
    /// default constructur
    RigidBody() : color(0, 1, 0) {}

    /// construct with a set of points and a total mass
    RigidBody(const std::vector<vec2>& _points, float _mass,
              const vec2 linVelocity = vec2(0, 0));

    /// after changing position and orientation,
    /// call this function to update particle positions
    void update_points();

public:
    vec2 position;          ///< position of the center of gravity (CoG)
    vec2 linear_velocity;   ///< linear velocity of CoG
    vec2 force;             ///< linear force
    float mass;             ///< total mass of the rigid body
    float orientation;      ///< the current angle of rotation
    float angular_velocity; ///< angular velocity
    float torque;           ///< torque
    float inertia;          ///< moment of inertia
    float radius;           ///< bounding sphere radius
    vec3 color;             ///< use to visualize body-body collisions

    std::vector<vec2> points; ///< vector of particles/points
    std::vector<vec2> r;      ///< vector of relative point positions
};

//=============================================================================
