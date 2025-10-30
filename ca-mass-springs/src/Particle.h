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

#include <pmp/MatVec.h>
using namespace pmp;

//== CLASS DEFINITION =========================================================

/** \class Particle Particle.h
 Class for a particle that stores position, velocity, force and other data.
 */
class Particle
{
public:
    /// constructor
    Particle(vec2 p, vec2 v, float m, bool l)
        : position(p),
          velocity(v),
          force(0, 0),
          mass(m),
          locked(l),
          acceleration(0, 0)
    {
    }

    vec2 position; ///< position of the particle
    vec2 velocity; ///< velocity of the particle
    vec2 force;    ///< accumulated force acting on the particle
    float mass;    ///< mass of the particle
    bool locked;   ///< is the particle locked?

    vec2 position_t;   ///< used for Midpoint integration
    vec2 velocity_t;   ///< used for Midpoint integration
    vec2 acceleration; ///< used for Verlet integration
};

//=============================================================================
