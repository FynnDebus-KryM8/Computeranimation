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

/** \class Triangle Triangle.h
 Class for storing triangles (for area preserving forces)
 */
class Triangle
{
public:
    /// construct with three particles. automatically computes rest area.
    Triangle(Particle &p0, Particle &p1, Particle &p2)
        : particle0(p0), particle1(p1), particle2(p2)
    {
        rest_area = area();
    }

    /// compute current area
    float area() const
    {
        const vec2 &p0 = particle0.position;
        const vec2 &p1 = particle1.position;
        const vec2 &p2 = particle2.position;
        return 0.5 * ((p1[0] - p0[0]) * (p2[1] - p0[1]) -
                      (p2[0] - p0[0]) * (p1[1] - p0[1]));
    }

    Particle &particle0; ///< reference of first particle
    Particle &particle1; ///< reference of second particle
    Particle &particle2; ///< reference of third particle
    float rest_area;     ///< area in rest state
};

//=============================================================================
