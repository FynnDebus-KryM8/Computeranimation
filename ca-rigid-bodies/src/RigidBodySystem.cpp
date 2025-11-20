//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "RigidBodySystem.h"
#include "simple_shader.h"
#include <float.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>

//== IMPLEMENTATION ==========================================================

vec3 random_color()
{
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return normalize(vec3(r, g, b));
}

//-----------------------------------------------------------------------------

RigidBodySystem::RigidBodySystem()
{
    vertexArray_ = 0;
    pointBuffer_ = 0;
    edgeBuffer_ = 0;
    wallBuffer_ = 0;
    multiple_bodies_mode_ = false;

    reset_parameters();

    // setup the four walls
    walls_.resize(4);
    walls_[0].p0 = walls_[3].p1 = vec2(-1, -1);
    walls_[1].p0 = walls_[0].p1 = vec2(1, -1);
    walls_[2].p0 = walls_[1].p1 = vec2(1, 1);
    walls_[3].p0 = walls_[2].p1 = vec2(-1, 1);
    for (auto &w : walls_)
        w.normal = normalize(perp(w.p1 - w.p0));
}

//-----------------------------------------------------------------------------

RigidBodySystem::~RigidBodySystem()
{
    glDeleteBuffers(1, &pointBuffer_);
    glDeleteBuffers(1, &edgeBuffer_);
    glDeleteBuffers(1, &wallBuffer_);
    glDeleteVertexArrays(1, &vertexArray_);
}

//-----------------------------------------------------------------------------

void RigidBodySystem::reset_parameters()
{
    time_step_ = 0.001;
    mass_ = 0.5;
    damping_ = 0.01;
    spring_stiffness_ = 100.0;
    spring_damping_ = 5.0;
    particle_radius_ = 0.03;
    use_gravity_ = true;
    use_linear_dynamics_ = true;
    use_angular_dynamics_ = true;
    mouse_spring_.active = false;
    collision_elasticity_ = 0.5;
    collision_damping_ = 2.0;
}

//-----------------------------------------------------------------------------

void RigidBodySystem::clear_bodies()
{
    bodies_.clear();
    update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::add_body(const std::vector<vec2> &points,
                               const vec2 linVelocity)
{
    bodies_.push_back(RigidBody(points, mass_, linVelocity));
    update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::add_mouse_spring(const vec2 p)
{
    int idx = -1;
    int idb = -1;
    float dmin = FLT_MAX;
    for (size_t j = 0; j < bodies_.size(); j++)
    {
        RigidBody &b = bodies_[j];
        for (size_t i = 0; i < b.points.size(); ++i)
        {
            float d = distance(p, b.points[i]);
            if (d < dmin)
            {
                dmin = d;
                idx = i;
                idb = j;
            }
        }
    }

    mouse_spring_.active = true;
    mouse_spring_.body_index = idb;
    mouse_spring_.mouse_position = p;
    mouse_spring_.particle_index = idx;

    update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::set_mouse_spring(vec2 p)
{
    if (mouse_spring_.active)
    {
        mouse_spring_.mouse_position = p;
        update_opengl_buffers();
    }
}

//-----------------------------------------------------------------------------

bool RigidBodySystem::is_mouse_spring_active() const
{
    return mouse_spring_.active;
}

//-----------------------------------------------------------------------------

void RigidBodySystem::clear_mouse_spring()
{
    mouse_spring_.particle_index = -1;
    mouse_spring_.active = false;
    update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::update_opengl_buffers()
{
    // generate buffers
    if (!vertexArray_)
    {
        glGenVertexArrays(1, &vertexArray_);
        glBindVertexArray(vertexArray_);
        glGenBuffers(1, &pointBuffer_);
        glGenBuffers(1, &edgeBuffer_);
        glGenBuffers(1, &fillBuffer_);
        glGenBuffers(1, &wallBuffer_);
        glGenBuffers(1, &collisionBuffer_);
    }
    glBindVertexArray(vertexArray_);

    int n = 0;
    for (const RigidBody &body : bodies_)
    {
        n += body.points.size();
    }

    // particle positions
    std::vector<vec2> pos;
    pos.reserve(n);
    for (const RigidBody &body : bodies_)
    {
        pos.insert(pos.end(), body.points.begin(), body.points.end());
    }
    if (mouse_spring_.active)
    {
        pos.push_back(mouse_spring_.mouse_position);
    }
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer_);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec2), pos.data(),
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // spring edges
    std::vector<GLuint> edges;
    edges.reserve(2 * n + 2);
    int j = 0;
    for (int i = 0; i < bodies_.size(); ++i)
    {
        const RigidBody &body = bodies_[i];
        int nb = body.points.size();

        for (int k = 0; k < nb; ++k)
        {
            edges.push_back(j + k);
            edges.push_back(j + ((k + 1) % nb));
        }

        if (mouse_spring_.active && mouse_spring_.body_index == i)
        {
            edges.push_back(mouse_spring_.particle_index + j);
            edges.push_back(n);
        }
        j += nb;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(GLuint),
                 edges.data(), GL_DYNAMIC_DRAW);

    // fill bodies with triangles
    std::vector<GLuint> triangles;
    for (int i = 0, j = 0; i < bodies_.size(); i++)
    {
        const int nb = bodies_[i].points.size();
        for (int k = 0; k < nb - 2; ++k)
        {
            triangles.push_back(j);
            triangles.push_back(j + k + 1);
            triangles.push_back(j + k + 2);
        }
        j += nb;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint),
                 triangles.data(), GL_DYNAMIC_DRAW);

    // walls for collision detection
    std::vector<vec2> wall;
    wall.reserve(10);
    float d0 = 1.0;
    float d1 = 1.1;
    if (multiple_bodies_mode_)
    {
        d0 = 1.0 + 1e-3 - particle_radius_;
    }
    for (int i = 0; i < 5; i++)
    {
        wall.push_back(walls_[i % 4].p0 * d0);
        wall.push_back(walls_[i % 4].p0 * d1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, wallBuffer_);
    glBufferData(GL_ARRAY_BUFFER, wall.size() * sizeof(vec2), wall.data(),
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

//-----------------------------------------------------------------------------

void RigidBodySystem::draw(const mat4 &projection)
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
    shader_.set_uniform("color", vec3(0.5, 0.5, 0.5));
    glBindBuffer(GL_ARRAY_BUFFER, wallBuffer_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

    // set point buffer
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // draw rigid body
    int n = 0;
    for (auto b : bodies_)
    {
        if (!b.points.empty())
        {
            // draw points
            shader_.set_uniform("use_lighting", true);
            shader_.set_uniform("color", vec3(0, 1, 0));
            for (auto p : b.points)
            {
                mat4 mvp = projection *
                           translation_matrix(vec3(p[0], p[1], 0.0)) *
                           scaling_matrix(particle_radius_);
                shader_.set_uniform("modelview_projection_matrix", mvp);

                if (!multiple_bodies_mode_)
                    sphere_.draw();
            }
            n += b.points.size();
        }
    }

    if (!bodies_.empty())
    {
        // draw edges
        shader_.set_uniform("modelview_projection_matrix", projection);
        shader_.set_uniform("use_lighting", false);
        shader_.set_uniform("color", vec3(0, 0, 0));
        glBindVertexArray(vertexArray_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeBuffer_);
        glDrawElements(GL_LINES, 2 * n + 2 * mouse_spring_.active,
                       GL_UNSIGNED_INT, NULL);
    }

    if (multiple_bodies_mode_)
    {
        // draw filled squares
        shader_.set_uniform("modelview_projection_matrix", projection);
        shader_.set_uniform("use_lighting", false);
        glBindVertexArray(vertexArray_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillBuffer_);

        int offset = 0;
        for (int i = 0; i < bodies_.size(); ++i)
        {
            shader_.set_uniform("color", bodies_[i].color);
            const int n = (bodies_[i].points.size() - 2) * 3;
            glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT,
                           (void *)(offset * sizeof(GLuint)));
            offset += n;
        }
    }

    glBindVertexArray(0);
    glCheckError();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::compute_forces()
{
    // clear forces
    for (auto &b : bodies_)
    {
        b.force = vec2(0.0, 0.0);
        b.torque = 0.0;
    }

    /**
     * \todo Compute the forces acting on the rigid body:
     * - Add gravity forces (depending on the switch `use_gravity`)
     * - Add damping forces
     */
    for (auto &b : bodies_)
    {
    }

    // mouse spring force
    if (mouse_spring_.active)
    {
        RigidBody &b = bodies_[mouse_spring_.body_index];

        vec2 p0 = b.points[mouse_spring_.particle_index];
        vec2 v0 =
            b.linear_velocity + b.angular_velocity * perp(p0 - b.position);
        vec2 p1 = mouse_spring_.mouse_position;
        vec2 d = p0 - p1;
        float l = norm(d);
        d /= l;
        vec2 f = -d * (spring_stiffness_ * l + spring_damping_ * dot(v0, d));

        // update force
        b.force += f;
        // update torque
        b.torque += dot(f, perp(p0 - b.position));
    }
}

//-----------------------------------------------------------------------------

void RigidBodySystem::time_integration()
{
    // compute all forces
    compute_forces();

    // handle body-wall collisions
    handle_wall_collisions();

    // handle body-body collisions
    handle_body_collisions();

    // update positions and velocities
    for (auto &b : bodies_)
    {
        /** \todo To implement explicit Euler time integration:
         * - Update position and orientation
         * - Update linear and angular velocities
         * - Optional: To make use of the GUI checkboxes, update linear parameters only
         *   if use_linear_dynamics_==true and angular ones if use_angular_dynamics_==true
         * - Hint: The class RigidBody stores all information needed for time-stepping
         */

        // update particle position after updating body position and orientation
        b.update_points();
    }

    // update OpenGL buffer for rendering
    update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void RigidBodySystem::handle_wall_collisions()
{
    for (auto &b : bodies_)
    {
        /**
         * \todo Handle collisions of body `b` and walls `walls_` based on impulses.
         * - Find out whether a rigid body is (partially) inside a wall
         * - Resolve collisions based on impulses if we have a colliding contact
         * - Hints:
         *      - The class `Wall` stores the data required for computing collisions
         *      - If you have more than one colliding point per body (e.g edge collision),
         *        handling all points individually will give the wrong result.
         *        Instead, compute the average collision point in this case.
         *      - You can use `perp(e)` to rotate a `vec2 e` by 90 degrees
         *        (in counter-clockwise order) to get a perpendicular vector
         *      - Since the wall does not move, compute v_rel only based on rigid body
         */
    }
}

//-----------------------------------------------------------------------------

void RigidBodySystem::handle_body_collisions()
{
    vec2 collision_point, collision_normal;

    for (unsigned int i = 0; i < bodies_.size(); i++)
    {
        for (unsigned int j = i + 1; j < bodies_.size(); j++)
        {
            RigidBody &b1 = bodies_[i];
            RigidBody &b2 = bodies_[j];

            if (detect_collision(b1, b2, collision_point, collision_normal))
            {
                resolve_collision(b1, b2, collision_point, collision_normal);
            }
        }
    }
}

//-----------------------------------------------------------------------------

bool RigidBodySystem::detect_collision(RigidBody &b1, RigidBody &b2,
                                       vec2 &collision_point,
                                       vec2 &collision_normal)
{
    collision_point = vec2(0, 0);
    collision_normal = vec2(0, 0);
    int counter = 0;
    std::vector<vec2> samples;
    vec2 n;

    // collect vertices and edge midpoints of first body and test them against second body
    samples = b1.points;
    for (int i = 0, N = b1.points.size(); i < N; i++)
    {
        samples.push_back(0.5 * (b1.points[i] + b1.points[(i + 1) % N]));
    }
    for (auto p : samples)
    {
        if (is_point_in_body(p, b2, n))
        {
            collision_point += p;
            collision_normal += n;
            counter++;
        }
    }

    // collect vertices and edge midpoints of second body and test them against first body
    samples = b2.points;
    for (int i = 0, N = b2.points.size(); i < N; i++)
    {
        samples.push_back(0.5 * (b2.points[i] + b2.points[(i + 1) % N]));
    }
    for (auto p : samples)
    {
        if (is_point_in_body(p, b1, n))
        {
            collision_point += p;
            collision_normal -= n;
            counter++;
        }
    }

    // if there is at least one collision -> find average collision point and normal
    if (counter > 0)
    {
        // visualize collision by changing color of both bodies
        b1.color = random_color();
        b2.color = random_color();

        // average collision point and normal
        collision_point /= (float)counter;
        collision_normal.normalize();

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

bool RigidBodySystem::is_point_in_body(const vec2 &p, const RigidBody &body,
                                       vec2 &normal)
{
    // THIS PART ASSUMES THAT THE RIGID BODY IS CONVEX

    /**
     * \todo Test whether point `p` is inside the convex object `body` and store the collision normal in `normal`.
     * Return true in case of a collision. 
     * Compute the signed distance to each (infinite line containing an) edge of the body. 
     * If `p` is inside with respect to each edge, there is a collision.
     *
     * Hints:
     * 1. You can compute the normal direction of each edge, defined by points `p1` and `p2`, via `n = perp(p1 - p2)`
     * 2. The collision normal is the normal of the closest edge to `p`.
     * 
     * Note: The code assumes that the rigid body is convex!
     */


    return false;
}

//-----------------------------------------------------------------------------

void RigidBodySystem::resolve_collision(RigidBody &b1, RigidBody &b2,
                                        const vec2 &collision_point,
                                        const vec2 &collision_normal)
{
    /**
     * \todo Handle impulse-based object-object collisions:
     * - Check the relative velocity to make sure that the objects do not already separate
     * - If there is a colliding collision, set `colliding=true` to trigger the object explosions.
     * - Apply the impulse-based collision response to both bodies.
     */

}

//=============================================================================
