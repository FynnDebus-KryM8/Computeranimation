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

#include "RigidBody.h"
#include "Sphere.h"

#include <pmp/Shader.h>
using namespace pmp;

#include <vector>
#include <set>

//== CLASS DEFINITION =========================================================

/** \class RigidBodySystem RigidBodySystem.h
Class for managing and simulating rigid bodies.
 */
class RigidBodySystem
{
public:
    /// Constructor
    RigidBodySystem();
    /// Destructor
    ~RigidBodySystem();

    /// Reset parameter values to their initial state
    void reset_parameters();

    /// Add rigid body
    void add_body(const std::vector<vec2> &points,
                  const vec2 linVelocity = vec2(0, 0));
    /// Remove all rigid bodies
    void clear_bodies();

    /// Render the rigid bodies
    void draw(const pmp::mat4 &projection);

    /// Perform one time step
    void time_integration();

    /// Remove mouse spring
    void clear_mouse_spring();
    /// Add mouse spring between mouse pos p and closest particle
    void add_mouse_spring(vec2 p);
    /// Set target of mouse spring to mouse pos p
    void set_mouse_spring(vec2 p);
    /// Is mouse spring active?
    bool is_mouse_spring_active() const;

private: //--- internal functions --------------------------------------------
    /// Compute all forces
    void compute_forces();

    /// Handle body-wall collisions
    void handle_wall_collisions();

    /// Handle body-body collisions
    /// Calls `object_object_collisions` for collision detection and response
    void handle_body_collisions();

    /// Determine whether body `b1` is colliding with body `b2`.
    /// Return collision point and collision normal via references.
    bool detect_collision(RigidBody &b1, RigidBody &b2, vec2 &collision_point,
                          vec2 &collision_normal);

    /// Impulse-based collisions response for body-body collisions
    void resolve_collision(RigidBody &b1, RigidBody &b2,
                           const vec2 &collidingPoint,
                           const vec2 &collisionNormal);

    /// Test whether a point `p` is inside a rigid object of a square `body`.
    /// Store the collision normal (normal of nearest edge) in `normal`
    bool is_point_in_body(const vec2 &p, const RigidBody &body, vec2 &normal);

public: //--- parameters -----------------------------------------------------
    /// radius of particles (for rendering and collisions)
    float particle_radius_;
    /// whether to use gravity force
    bool use_gravity_;
    /// whether to use linear dynamics
    bool use_linear_dynamics_;
    /// whether to use angular dynamics
    bool use_angular_dynamics_;
    /// whether to split colliding bodies
    bool use_body_explosion_;
    /// time step of the simulation
    float time_step_;
    /// total mass of the rigid body
    float mass_;
    /// damping coefficient (friction body-air)
    float damping_;
    /// damping for collisions (friction body-wall/body-body)
    float collision_damping_;
    /// stiffness of interactive mouse spring
    float spring_stiffness_;
    /// damping of interactive mouse spring
    float spring_damping_;
    /// how much velocity will be decreased after collision
    float collision_elasticity_;

public: //--- simulation data ------------------------------------------------
    /// the rigid bodies to be simulated
    std::vector<RigidBody> bodies_;

    /// struct holding all parameters of a wall
    struct Wall
    {
        vec2 p0;     ///< first endpoint
        vec2 p1;     ///< second endpoint
        vec2 normal; ///< wall normal (pointing inside the simulation area)
    };
    std::vector<Wall> walls_;

    /// whether we are in multiple bodies mode (key 4)
    bool multiple_bodies_mode_;

private:
    /// the interactive spring controlled by the mouse
    struct
    {
        /// position of the mouse cursor (one endpoint of spring)
        vec2 mouse_position;
        /// which rigid body is the other endpoint
        int body_index;
        /// which particle is the other endpoint
        int particle_index;
        /// is the spring active?
        bool active;
    } mouse_spring_;

private: //--- OpenGL rendering ----------------------------------------------
    /// send all data that should be drawn to the GPU
    void update_opengl_buffers();

    Shader shader_;
    Sphere sphere_;

    GLuint vertexArray_;
    GLuint pointBuffer_;
    GLuint edgeBuffer_;
    GLuint wallBuffer_;
    GLuint fillBuffer_;
    GLuint collisionBuffer_;
};

//=============================================================================
