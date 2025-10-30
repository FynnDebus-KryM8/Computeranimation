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

#include <Particle.h>

//== CLASS DEFINITION =========================================================

/** \class Spring Spring.h
 Class for representing springs
 */
class Spring
{
public:
    /// construct with two particles. automatically computes rest length.
    Spring(Particle &p0, Particle &p1) : particle0(p0), particle1(p1)
    {
        rest_length = length();
    }

    /// get current length of the spring
    float length() const
    {
        return norm(particle0.position - particle1.position);
    }

    Particle &particle0; ///< reference of first particle
    Particle &particle1; ///< reference of second particle
    float rest_length;   ///< rest length
};

//=============================================================================
