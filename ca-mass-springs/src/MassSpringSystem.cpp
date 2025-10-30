//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "MassSpringSystem.h"
#include "simple_shader.h"
#include <fstream>

//== MASS SPRING IMPLEMENTATION ==============================================

MassSpringSystem::MassSpringSystem()
{
    vertexArray_ = 0;
    particleBuffer_ = 0;
    springBuffer_ = 0;
    triangleBuffer_ = 0;
    wallBuffer_ = 0;

    reset_parameters();
}

//-----------------------------------------------------------------------------

MassSpringSystem::~MassSpringSystem()
{
    glDeleteBuffers(1, &particleBuffer_);
    glDeleteBuffers(1, &springBuffer_);
    glDeleteBuffers(1, &triangleBuffer_);
    glDeleteBuffers(1, &wallBuffer_);
    glDeleteVertexArrays(1, &vertexArray_);
}

//-----------------------------------------------------------------------------

void MassSpringSystem::reset_parameters()
{
#if __EMSCRIPTEN__
    // use larger time-step for web-demo
    time_step_ = 0.001;
#else
    // smaller time-step for exercises
    time_step_ = 0.0005;
#endif
    integration_ = Euler;

    particle_radius_ = 0.03;
    particle_mass_ = 0.1;

    use_gravity_ = true;
    damping_ = 0.1;

    collisions_ = Force_based;
    collision_stiffness_ = 1000.0;
    collision_damping_ = 0.1;

    spring_stiffness_ = 1000.0;
    spring_damping_ = 1.0;
    area_stiffness_ = 100000.0;

    mouse_spring_.active = false;
    mouse_spring_.stiffness = 0.25f * spring_stiffness_;
    mouse_spring_.damping = spring_damping_;
}

//-----------------------------------------------------------------------------

void MassSpringSystem::clear()
{
    particles.clear();
    springs.clear();
    triangles.clear();
    mouse_spring_.active = false;
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::add_particle(vec2 position, vec2 velocity, bool locked)
{
    particles.push_back(Particle(position, velocity, particle_mass_, locked));
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::add_spring(unsigned int i0, unsigned int i1)
{
    assert(i0 < particles.size());
    assert(i1 < particles.size());
    springs.push_back(Spring(particles[i0], particles[i1]));
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::add_triangle(unsigned int i0, unsigned int i1,
                                    unsigned int i2)
{
    assert(i0 < particles.size());
    assert(i1 < particles.size());
    assert(i2 < particles.size());
    triangles.push_back(Triangle(particles[i0], particles[i1], particles[i2]));
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

int MassSpringSystem::get_nearest_particle(const vec2 p) const
{
    int pidx = -1;
    float dmin = FLT_MAX;
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
        float d = sqrnorm(p - particles[i].position);
        if (d < dmin)
        {
            dmin = d;
            pidx = i;
        }
    }
    return pidx;
}

//-----------------------------------------------------------------------------

void MassSpringSystem::add_mouse_spring(vec2 p)
{
    mouse_spring_.mouse_position = p;
    mouse_spring_.particle_index = get_nearest_particle(p);
    mouse_spring_.active = true;
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::set_mouse_spring(vec2 p)
{
    if (mouse_spring_.active)
    {
        mouse_spring_.mouse_position = p;
        updateOpenGLBuffers();
    }
}

//-----------------------------------------------------------------------------

bool MassSpringSystem::is_mouse_spring_active() const
{
    return mouse_spring_.active;
}

//-----------------------------------------------------------------------------

void MassSpringSystem::clear_mouse_spring()
{
    mouse_spring_.particle_index = -1;
    mouse_spring_.active = false;
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::updateOpenGLBuffers()
{
    // generate buffers
    if (!vertexArray_)
    {
        glGenVertexArrays(1, &vertexArray_);
        glBindVertexArray(vertexArray_);
        glGenBuffers(1, &particleBuffer_);
        glGenBuffers(1, &springBuffer_);
        glGenBuffers(1, &triangleBuffer_);
        glGenBuffers(1, &wallBuffer_);
    }
    glBindVertexArray(vertexArray_);

    // particle positions
    std::vector<vec2> pos;
    pos.reserve(particles.size()+1);
    for (const Particle& particle: particles)
    {
        pos.push_back(particle.position);
    }
    if (mouse_spring_.active)
    {
        pos.push_back(mouse_spring_.mouse_position);
    }
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec2), pos.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // spring edges
    std::vector<GLuint> edges;
    edges.reserve(2 * springs.size() + 2);
    for (const Spring& spring: springs)
    {
        edges.push_back(&spring.particle0 - &particles[0]);
        edges.push_back(&spring.particle1 - &particles[0]);
    }
    if (mouse_spring_.active)
    {
        edges.push_back(mouse_spring_.particle_index);
        edges.push_back(particles.size());
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, springBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(GLuint),
                 edges.data(), GL_STATIC_DRAW);

    // triangles
    std::vector<GLuint> tri;
    tri.reserve(3 * triangles.size());
    for (const Triangle& triangle: triangles)
    {
        tri.push_back(&triangle.particle0 - &particles[0]);
        tri.push_back(&triangle.particle1 - &particles[0]);
        tri.push_back(&triangle.particle2 - &particles[0]);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tri.size() * sizeof(GLuint),
                 tri.data(), GL_STATIC_DRAW);

    // walls for collision detection
    std::vector<vec2> wall;
    wall.push_back(vec2(-1.0, 1.0));
    wall.push_back(vec2(-1.0, -1.0));
    wall.push_back(vec2(1.0, -1.0));
    wall.push_back(vec2(1.0, 1.0));
    wall.push_back(vec2(-1.0, 1.0));
    glBindBuffer(GL_ARRAY_BUFFER, wallBuffer_);
    glBufferData(GL_ARRAY_BUFFER, wall.size() * sizeof(vec2), wall.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

//-----------------------------------------------------------------------------

void MassSpringSystem::draw(const mat4& projection)
{
    // setup vertex array
    if (!vertexArray_)
        return;
    glBindVertexArray(vertexArray_);

    // initialize shader
    if (!shader_.is_valid())
        if (!shader_.source(phong_vshader, phong_fshader))
            exit(1);

    // set shader parameters
    shader_.use();
    shader_.set_uniform("use_lighting", false);
    shader_.set_uniform("modelview_projection_matrix", projection);

    // draw walls
    {
        shader_.set_uniform("color", vec3(0.5, 0.5, 0.5));
        glBindBuffer(GL_ARRAY_BUFFER, wallBuffer_);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // draw particles
    if (particles.size())
    {
        shader_.set_uniform("use_lighting", true);
        for (auto p : particles)
        {
            shader_.set_uniform("color",
                                p.locked ? vec3(1, 0, 0) : vec3(0, 1, 0));
            mat4 mvp =
                projection *
                translation_matrix(vec3(p.position[0], p.position[1], 0.0)) *
                scaling_matrix(particle_radius_);
            shader_.set_uniform("modelview_projection_matrix", mvp);
            sphere_.draw();
        }
        shader_.set_uniform("modelview_projection_matrix", projection);
        glBindVertexArray(vertexArray_);
    }

    // draw springs
    if (springs.size() || mouse_spring_.active)
    {
        shader_.set_uniform("use_lighting", false);
        shader_.set_uniform("color", vec3(0, 0, 0));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, springBuffer_);
        glDrawElements(GL_LINES, 2 * springs.size() + 2 * mouse_spring_.active,
                       GL_UNSIGNED_INT, NULL);
    }

    // draw triangles
    if (triangles.size())
    {
        shader_.set_uniform("use_lighting", false);
        shader_.set_uniform("color", vec3(0.8, 1.0, 0.8));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleBuffer_);
        glDepthRange(0.01, 1.0);
        glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT,
                       NULL);
        glDepthRange(0.0, 1.0);
    }

    glBindVertexArray(0);
    glCheckError();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::compute_forces()
{
    // clear forces
    for (Particle& p : particles)
        p.force = vec2(0, 0);

    // gravity force
    if (use_gravity_)
        for (Particle& p : particles)
            p.force += vec2(0.0, -9.81) * particle_mass_;

    /** \todo Compute and sum up all forces for each particle.
     * - gravity is already computed above
     * - compute damping forces
     *   use damping_ as the magic constant (gamma in the slides)
     * - compute collision forces (if `collisions_ == Force_based`)
     *   the normal vectors of the four planes are easy to figure out,
     *   and they have distance 1 from the origin (0,0), 
     *   which is the center of the box). Use collision_stiffness_
     *   as the magic constant (k_coll in the slides).
     * - maybe you want to add an additional damping term in case of a collision
     *   to get a less slippery behavior on the ground. Use 10.0*collision_damping_
     *   as the magic constant.
     * - compute spring forces (including the damping term). use spring_stiffness_ 
     *   and spring_damping_ as magic constants (called k_s and k_d in the slides)
     * - compute triangle-area forces. use area_stiffness_ as magic constant.
     * - compute force of the interactive `mouse_spring_` 
     *   (if `mouse_spring_.active == true`), which is a spring of target length 0
     *   connecting the mouse pointer with the selected particle. use
     *   mouse_spring_.stiffness and mouse_spring_.damping as magic constants.
     * 
     * See also `mouse_spring_`, `collisions_`, and `area_forces_`.
     */
}

//-----------------------------------------------------------------------------

void MassSpringSystem::time_integration()
{
    float dt = time_step_;

    switch (integration_)
    {
        case Euler:
        {
            // compute all forces
            compute_forces();

            // update positions
            for (Particle& p: particles)
                if (!p.locked)
                    p.position += dt * p.velocity;

            // update velocities
            for (Particle& p: particles)
                if (!p.locked)
                    p.velocity += dt * p.force / p.mass;

            break;
        }

        case Midpoint:
        {
            /**
             * \todo Implement the Midpoint time integration scheme.
             * - The Particle class has variables `position_t` and `velocity_t` to store
             *   current values.
             * - Hint: `compute_forces()` computes all forces for the current positions and
             *   velocities.
             */
            break;
        }

        case Verlet:
        {
            /**
             * \todo Implement the Velocity Verlet time integration scheme.
             * - The Particle class has a variable acceleration to remember the 
             *   acceleration of the previous time step
             * - `compute_forces()` computes all forces for the current positions and
             *   velocities.
             */
            break;
        }
    }

    // impulse-based collision handling
    if (collisions_ == Impulse_based)
    {
        impulse_based_collisions();
    }

    // finally update OpenGL buffers
    updateOpenGLBuffers();
}

//-----------------------------------------------------------------------------

void MassSpringSystem::impulse_based_collisions()
{
    /** \todo Handle collisions based on impulses
     *   - detect whether a particle collides with one of the walls
     *   - detect whether the contact is colliding, i.e., whether
     *     the velocity vector points inside the wall
     *   - the normal vectors of the four planes are easy to figure out,
     *     and they have distance 1 from the origin (0,0), 
     *     which is the center of the box).
     *   - mirror the velocity vector at the wall normal
     *   - add a bit of damping, i.e., scale the resulting velocity vector 
     *     by a constant slightly smaller than 1. Use (1.0-collision_damping_)
     *     as the magic constant.
     * 
    */
}

//=============================================================================
