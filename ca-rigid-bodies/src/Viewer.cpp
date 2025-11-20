//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include <Viewer.h>
#include <pmp/GL.h>
#include <imgui.h>
#include <chrono>

using namespace pmp;
using namespace std::chrono;

//== IMPLEMENTATION ==========================================================

Viewer::Viewer(const char* _title, int _width, int _height)
    : Window(_title, _width, _height)
{
    animate_ = false;
    keyboard('1', 0, GLFW_PRESS, 0);
    simulation_time_ = 100.0;

    clear_help_items();
    add_help_item("1-4", "Initialize different scenes");
    add_help_item("Space", "Start/stop simulation");
    add_help_item("S", "Single time step");
    add_help_item("Left mouse", "Drag mass points");
    add_help_item("Backspace", "Reset parameters");
    add_help_item("G", "Toggle GUI dialog");
    add_help_item("PageUp/Down", "Scale GUI dialogs");
#ifndef __EMSCRIPTEN__
    add_help_item("Esc/Q", "Quit application");
#endif
}

//-----------------------------------------------------------------------------

void Viewer::keyboard(int key, int code, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    static int lastmode = -1;
    switch (key)
    {
        // setup problem 1
        case '1':
        {
            simulation_.clear_bodies();

            std::vector<vec2> p;
            p.push_back(vec2(-0.6, -0.6));
            p.push_back(vec2(-0.4, -0.6));
            p.push_back(vec2(-0.4, -0.4));
            p.push_back(vec2(-0.6, -0.4));
            simulation_.add_body(p);
            simulation_.multiple_bodies_mode_ = false;

            if (lastmode == '4')
                simulation_.reset_parameters();
            lastmode = '1';
            break;
        }

        // setup problem 2
        case '2':
        {
            simulation_.clear_bodies();

            std::vector<vec2> p;
            p.push_back(vec2(-0.3, -0.1));
            p.push_back(vec2(-0.1, -0.1));
            p.push_back(vec2(0.1, -0.1));
            p.push_back(vec2(0.3, -0.1));
            p.push_back(vec2(0.3, 0.1));
            p.push_back(vec2(0.1, 0.1));
            p.push_back(vec2(-0.1, 0.1));
            p.push_back(vec2(-0.3, 0.1));
            simulation_.add_body(p);
            simulation_.multiple_bodies_mode_ = false;

            if (lastmode == '4')
                simulation_.reset_parameters();
            lastmode = '2';
            break;
        }

        // setup problem 3
        case '3':
        {
            simulation_.clear_bodies();

            std::vector<vec2> p;
            p.push_back(vec2(-0.5, 0.1));
            p.push_back(vec2(-0.5, 0.0));
            p.push_back(vec2(0.0, 0.0));
            p.push_back(vec2(0.0, -0.3));
            p.push_back(vec2(0.1, -0.3));
            p.push_back(vec2(0.1, 0.0));
            p.push_back(vec2(0.3, 0.0));
            p.push_back(vec2(0.3, 0.1));
            simulation_.add_body(p);
            simulation_.multiple_bodies_mode_ = false;

            if (lastmode == '4')
                simulation_.reset_parameters();
            lastmode = '3';
            break;
        }

        // setup problem 4
        case '4':
        {
            if (lastmode != '4')
            {
                simulation_.use_gravity_ = false;
                simulation_.damping_ = 0.0;
                simulation_.collision_elasticity_ = 0.9;
                simulation_.collision_damping_ = 0.0;
            }

            simulation_.clear_bodies();

            std::vector<vec2> p;
            p.push_back(vec2(-0.6, -0.6));
            p.push_back(vec2(-0.4, -0.6));
            p.push_back(vec2(-0.4, -0.4));
            simulation_.add_body(p, vec2(0.2, 0.5));

            std::vector<vec2> p2;
            p2.push_back(vec2(0.6, 0.6));
            p2.push_back(vec2(0.4, 0.6));
            p2.push_back(vec2(0.4, 0.4));
            p2.push_back(vec2(0.6, 0.4));
            simulation_.add_body(p2, vec2(0.4, 0.3));

            std::vector<vec2> p3;
            p3.push_back(vec2(0.7, -0.4));
            p3.push_back(vec2(0.5, -0.25));
            p3.push_back(vec2(0.3, -0.4));
            p3.push_back(vec2(0.4, -0.6));
            p3.push_back(vec2(0.6, -0.6));
            simulation_.add_body(p3, vec2(-0.4, 0.3));

            simulation_.multiple_bodies_mode_ = true;

            lastmode = '4';
            break;
        }

        // toggle animation
        case GLFW_KEY_SPACE:
        {
            animate_ = !animate_;
            break;
        }

        // toggle animation
        case GLFW_KEY_BACKSPACE:
        {
            simulation_.reset_parameters();
            break;
        }

        // perform a single time step
        case GLFW_KEY_S:
        {
            simulation_.time_integration();
            break;
        }

        // perform 100 time steps
        case GLFW_KEY_T:
        {
            for (int i = 0; i < 100; i++)
                simulation_.time_integration();
            break;
        }

        // let parent do the work
        default:
        {
            pmp::Window::keyboard(key, code, action, mods);
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::do_processing()
{
    static auto before = high_resolution_clock::now();

    if (animate_)
    {
        // how many milliseconds since last computation?
        auto current = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(current - before).count();

        // let's do 20 time steps after 15ms (which is approximately 60Hz)
        if (elapsed > 15)
        {
            before = current;
            for (int i = 0; i < 20; ++i)
            {
                simulation_.time_integration();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::process_imgui()
{
    if (ImGui::CollapsingHeader("Time Integration",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        // animation checkbox
        ImGui::Checkbox("Animate it!", &animate_);

        ImGui::Spacing();
        ImGui::Spacing();

        // time step
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Time Step", &simulation_.time_step_, 0.00005f,
                           0.02f, "%.5f", 2);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Forces", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::Checkbox("Linear Dynamics", &simulation_.use_linear_dynamics_);
        ImGui::Checkbox("Angular Dynamics", &simulation_.use_angular_dynamics_);
        ImGui::Checkbox("Gravity", &simulation_.use_gravity_);

        ImGui::PushItemWidth(100);

        ImGui::Spacing();
        ImGui::Text("Damping:");
        ImGui::SliderFloat("##Damping", &simulation_.damping_, 0.0f, 1.0f,
                           "%.2f", 2.0);

        ImGui::Spacing();
        ImGui::Text("Collision Elasticity:");
        ImGui::SliderFloat("##Coll-Elasticity",
                           &simulation_.collision_elasticity_, 0.0f, 1.0f,
                           "%.2f");

        ImGui::Spacing();
        ImGui::Text("Collision Damping:");
        ImGui::SliderFloat("##Collision Damping",
                           &simulation_.collision_damping_, 0.0f, 20.0f, "%.2f",
                           2.0);

        ImGui::PopItemWidth();
    }
}

//-----------------------------------------------------------------------------

void Viewer::display()
{
    // OpenGL state
    glEnable(GL_DEPTH_TEST);

    // clear screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // adjust viewport
    int gui_width = 200, width_wo_gui = width() - gui_width;
    glViewport(gui_width, 0, width_wo_gui, height());

    // adjust projection
    projection_matrix_ =
        (width_wo_gui > height())
            ? ortho_matrix(-1.1f * (float)width_wo_gui / (float)height(),
                           1.1f * (float)width_wo_gui / (float)height(), -1.1f,
                           1.1f)
            : ortho_matrix(-1.1f, 1.1f,
                           -1.1f * (float)height() / (float)width_wo_gui,
                           1.1f * (float)height() / (float)width_wo_gui);

    // draw particles, springs, triangles, ...
    simulation_.draw(projection_matrix_);
}

//-----------------------------------------------------------------------------

vec2 Viewer::pick(int x, int y)
{
    // get viewport data
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // in OpenGL y=0 is at the 'bottom'
    y = viewport[3] - y;

    // read depth buffer value at (x, y_new)
    float xf =
        ((float)x - (float)viewport[0]) / ((float)viewport[2]) * 2.0f - 1.0f;
    float yf =
        ((float)y - (float)viewport[1]) / ((float)viewport[3]) * 2.0f - 1.0f;
    float zf = 0.0;
    zf = zf * 2.0f - 1.0f;

    mat4 mvp = projection_matrix_;
    mat4 inv = inverse(mvp);
    vec4 p = inv * vec4(xf, yf, zf, 1.0f);
    p /= p[3];

    return vec2(p[0], p[1]);
}

//----------------------------------------------------------------------------

void Viewer::mouse(int _button, int _action, int _mods)
{
    if (!simulation_.bodies_.empty())
    {
        // release current mouse spring
        if (_button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_RELEASE)
        {
            simulation_.clear_mouse_spring();
        }

        // mouse button press generates new mouse spring
        else if (_button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_PRESS)
        {
            // get point under mouse cursor
            double x, y;
            cursor_pos(x, y);
            vec2 p = pick(x, y);

            // activate mouse spring
            simulation_.add_mouse_spring(p);
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::motion(double _x, double _y)
{
    if (simulation_.is_mouse_spring_active())
    {
        // update mouse positions
        simulation_.set_mouse_spring(pick(_x, _y));
    }
}

//=============================================================================
