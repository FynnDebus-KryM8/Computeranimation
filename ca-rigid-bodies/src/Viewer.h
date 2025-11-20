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

#include "RigidBodySystem.h"

#include <pmp/Window.h>
#include <pmp/Shader.h>
using namespace pmp;

#include <string>
#include <vector>

//== CLASS DEFINITION =========================================================

/** \class Viewer Viewer.h
 A 2D viewer for rigid-body-simulations.
 By pressing the space bar the user can turn on/off the simulation.
 When in "play" mode this class automatically triggers the time_integration()
 function of the RigidBodySystem-class.
 */

class Viewer : public pmp::Window
{
public:
    /// constructor
    Viewer(const char* _title, int _width, int _height);

private: // GUI functions
    /// render the scene
    virtual void display() override;

    /// handle keyboard events
    virtual void keyboard(int key, int code, int action, int mods) override;

    /// this function handles mouse button events
    virtual void mouse(int button, int action, int mods) override;

    /// this function handles mouse motion (passive/active position)
    virtual void motion(double x, double y) override;

    /// this function triggers the time_integration() function
    virtual void do_processing() override;

    /// render/handle GUI
    virtual void process_imgui() override;

    /// pick a 2D point by mouse clicking
    vec2 pick(int _x, int _y);

private: // simulation data and settings
    /// the mass spring system to be simulated
    RigidBodySystem simulation_;

    /// is animation on/off?
    bool animate_;

    /// OpenGL stuff
    mat4 projection_matrix_;

    /// simulation time
    double simulation_time_;

    /// selected particle for mouse spring
    int mouse_spring_;
};

//=============================================================================
