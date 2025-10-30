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
#include <Spring.h>
#include <Triangle.h>
#include <Sphere.h>

#include <pmp/Shader.h>
using namespace pmp;

#include <vector>
#include <cfloat>

//== CLASS DEFINITION =========================================================

/** \class MassSpringSystem MassSpringSystem.h
 Class for managing a mass-spring system
 */
class MassSpringSystem
{
public:
    /// constructor
    MassSpringSystem();

    /// destructor
    ~MassSpringSystem();

    /// reset parameter values to their initial state
    void reset_parameters();

    /// clear all particles, springs, and triangles
    void clear();

    /// add a particle
    void add_particle(vec2 position, vec2 velocity, bool locked);

    /// add a spring
    void add_spring(unsigned int i0, unsigned int i1);

    /// add a triangle
    void add_triangle(unsigned int i0, unsigned int i1, unsigned int i2);

    /// remove mouse spring
    void clear_mouse_spring();
    /// add mouse spring between mouse pos p and closest particle
    void add_mouse_spring(vec2 p);
    /// set target of mouse spring to mouse pos p
    void set_mouse_spring(vec2 p);
    /// is mouse spring active?
    bool is_mouse_spring_active() const;

    /// return index of closest particle
    int get_nearest_particle(const vec2 p) const;

    /// render the mass spring system
    void draw(const pmp::mat4& projection);

    /// perform one time step using either Euler, Midpoint, or Verlet
    void time_integration();

private: //--- internal functions --------------------------------------------
    /// compute all external and internal forces
    void compute_forces();

    /// perform impulse-based collision handling
    void impulse_based_collisions();

public: //--- parameters -----------------------------------------------------
    /// value of time-step
    float time_step_;

    /// parameter: mass of a particle
    float particle_mass_;
    /// parameter: radius of particles (for rendering and collisions)
    float particle_radius_;

    /// parameter: use gravity force?
    bool use_gravity_;

    /// parameter: amount of damping
    float damping_;

    /// parameter: strength of collision forces
    float collision_stiffness_;
    /// parameter: amount of wall damping (dissipation in case of collision)
    float collision_damping_;

    /// parameter: stiffness of springs
    float spring_stiffness_;
    /// parameter: internal damping of springs
    float spring_damping_;

    /// parameter: strength of area-preserving forces
    float area_stiffness_;

    /// paramters: which time-integration to use
    enum
    {
        Euler = 0,
        Midpoint = 1,
        Verlet = 2
    } integration_;

    /// parameter: how to handle collisiont
    enum
    {
        No_collisions = 0,
        Force_based = 1,
        Impulse_based = 2
    } collisions_;

public: //--- simulation data ------------------------------------------------
    std::vector<Particle> particles; ///< vector of all particles
    std::vector<Spring> springs;     ///< vector of all springs
    std::vector<Triangle> triangles; ///< vector of all triangles

private:
    /// the interactive spring controlled by the mouse
    struct MouseSpring
    {
        /// position of the mouse cursor (one endpoint of spring)
        vec2 mouse_position;
        /// which particle is the other endpoint
        int particle_index;
        /// is the spring active?
        bool active;
        /// stiffness
        float stiffness;
        /// damping
        float damping;
    } mouse_spring_;

private: //--- OpenGL rendering ----------------------------------------------
    /// update OpenGL buffers
    void updateOpenGLBuffers();

    Shader shader_;
    Sphere sphere_;

    GLuint vertexArray_;
    GLuint particleBuffer_;
    GLuint springBuffer_;
    GLuint triangleBuffer_;
    GLuint wallBuffer_;
};

//=============================================================================
