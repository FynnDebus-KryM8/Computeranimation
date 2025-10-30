//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "Sphere.h"

#include <pmp/MatVec.h>
using namespace pmp;

#include <vector>
#include <math.h>

//=============================================================================

Sphere::Sphere(unsigned int resolution)
    : resolution_(resolution),
      n_indices_(0),
      vertexArray_(0),
      vertexBuffer_(0),
      normalBuffer_(0),
      indexBuffer_(0)
{
}

//-----------------------------------------------------------------------------

Sphere::~Sphere()
{
    if (vertexBuffer_)
        glDeleteBuffers(1, &vertexBuffer_);
    if (normalBuffer_)
        glDeleteBuffers(1, &normalBuffer_);
    if (indexBuffer_)
        glDeleteBuffers(1, &indexBuffer_);
    if (vertexArray_)
        glDeleteVertexArrays(1, &vertexArray_);
}

//-----------------------------------------------------------------------------

void Sphere::initialize()
{
    const int v_resolution = resolution_;
    const int u_resolution = 2 * resolution_;
    const int n_vertices = v_resolution * u_resolution;
    const int n_triangles = 2 * (v_resolution - 1) * (u_resolution - 1);

    GLfloat *positions = new GLfloat[3 * n_vertices];
    GLfloat *normals = new GLfloat[3 * n_vertices];
    GLuint *indices = new GLuint[3 * n_triangles];

    unsigned int p(0), n(0), i(0);

    // generate vertices
    for (int iv = 0; iv < v_resolution; ++iv)
    {
        for (int iu = 0; iu < u_resolution; ++iu)
        {
            float u = (float)iu / (float)(u_resolution - 1);
            float v = (float)iv / (float)(v_resolution - 1);

            float theta = 2.0 * M_PI * u;
            float phi = M_PI * v;

            vec3 P(cos(theta) * sin(phi), cos(phi), sin(theta) * sin(phi));
            vec3 U(-sin(theta) * sin(phi), 0.0, cos(theta) * sin(phi));
            vec3 V(cos(theta) * cos(phi), -sin(phi), sin(theta) * cos(phi));

            U = normalize(U);
            V = normalize(V);

            positions[p++] = P[0];
            positions[p++] = P[1];
            positions[p++] = P[2];

            normals[n++] = P[0];
            normals[n++] = P[1];
            normals[n++] = P[2];
        }
    }

    // generate triangles
    for (int v = 0; v < v_resolution - 1; ++v)
    {
        for (int u = 0; u < u_resolution - 1; ++u)
        {
            unsigned int i0 = (u) + (v)*u_resolution;
            unsigned int i1 = (u + 1) + (v)*u_resolution;
            unsigned int i2 = (u + 1) + (v + 1) * u_resolution;
            unsigned int i3 = (u) + (v + 1) * u_resolution;

            indices[i++] = i0;
            indices[i++] = i1;
            indices[i++] = i2;

            indices[i++] = i0;
            indices[i++] = i2;
            indices[i++] = i3;
        }
    }
    n_indices_ = 3 * n_triangles;

    // generate vertex array object
    glGenVertexArrays(1, &vertexArray_);
    glBindVertexArray(vertexArray_);

    // vertex positions -> attribute 0
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, 3 * n_vertices * sizeof(float), positions,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // normal vectors -> attribute 1
    glGenBuffers(1, &normalBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
    glBufferData(GL_ARRAY_BUFFER, 3 * n_vertices * sizeof(float), normals,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // triangle indices
    glGenBuffers(1, &indexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * n_triangles * sizeof(GLuint),
                 indices, GL_STATIC_DRAW);

    // clean up
    delete[] positions;
    delete[] normals;
    delete[] indices;
}

//-----------------------------------------------------------------------------

void Sphere::draw()
{
    if (n_indices_ == 0)
        initialize();
    glBindVertexArray(vertexArray_);
    glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

//=============================================================================
