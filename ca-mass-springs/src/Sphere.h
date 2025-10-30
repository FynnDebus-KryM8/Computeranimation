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

#include <pmp/GL.h>

//=============================================================================

class Sphere
{
public:
    // constructor
    Sphere(unsigned int resolution = 10);

    // destructor
    ~Sphere();

    // render mesh
    void draw();

private:
    // generate sphere vertices/triangles and OpenGL buffers
    void initialize();

private:
    // tessellation resolution
    unsigned int resolution_;
    unsigned int n_indices_;

    // buffer indices
    GLuint vertexArray_;
    GLuint vertexBuffer_, normalBuffer_;
    GLuint indexBuffer_;
};

//=============================================================================
