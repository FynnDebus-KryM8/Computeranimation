//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Animation"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright  Computer Graphics Group, TU Dortmund.
//
//=============================================================================

//== INCLUDES =================================================================

#include <Viewer.h>
#include <pmp/GL.h>
#include <imgui.h>
#include <chrono>

using namespace pmp;

//== IMPLEMENTATION ==========================================================

Viewer::Viewer(const char* _title, int _width, int _height)
    : Window(_title, _width, _height)
{
    animate_ = false;
    keyboard('3', 0, GLFW_PRESS, 0);

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

    switch (key)
    {
        // setup problem 1
        case '1':
        {
            body_.clear();
            body_.add_particle(vec2(-0.8, -0.8), vec2(5.0, 5.0), false);
            break;
        }

        // setup problem 2
        case '2':
        {
            body_.clear();
            body_.add_particle(vec2(-0.1, 0.7), vec2(0.0, 0.0), false);
            body_.add_particle(vec2(0.0, 0.6), vec2(0.0, 0.0), false);
            body_.add_particle(vec2(0.1, 0.7), vec2(0.0, 0.0), false);
            body_.add_spring(0, 1);
            body_.add_spring(0, 2);
            body_.add_spring(1, 2);
            body_.add_triangle(0, 1, 2);
            break;
        }

        // setup problem 3
        case '3':
        {
            body_.clear();
            for (int i = 0; i < 8; ++i)
            {
                body_.add_particle(vec2(-0.5 + 0.2 * cos(0.25 * i * M_PI),
                                        -0.5 + 0.2 * sin(0.25 * i * M_PI)),
                                   vec2(5.0, 5.0), false);
            }
            body_.add_particle(vec2(-0.5, -0.5), vec2(5.0, 5.0), false);
            for (unsigned int i = 0; i < 8; ++i)
            {
                body_.add_spring(i, (i + 1) % 8);
                body_.add_spring(i, 8);
                body_.add_triangle(i, (i + 1) % 8, 8);
            }
            break;
        }

        // setup problem 4
        case '4':
        {
            body_.clear();
            for (int i = 0; i < 10; ++i)
            {
                body_.add_particle(vec2(i * 0.1, 0.8), vec2(0.0, 0.0), i == 0);
            }
            for (unsigned int i = 0; i < 9; ++i)
            {
                body_.add_spring(i, i + 1);
            }
            break;
        }

        // toggle animation
        case GLFW_KEY_SPACE:
        {
            animate_ = !animate_;
            break;
        }

        // reset parameters of current scene
        case GLFW_KEY_BACKSPACE:
        {
            body_.reset_parameters();
            break;
        }

        // perform a single time step
        case GLFW_KEY_S:
        {
            body_.time_integration();
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
    static auto before = std::chrono::high_resolution_clock::now();

    if (animate_)
    {
        // how many milliseconds since last computation?
        auto current = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           current - before)
                           .count();

        // let's do 20 time steps after 15ms (which is approximately 60Hz)
        if (elapsed > 15)
        {
            for (int i = 0; i < 20; ++i)
            {
                body_.time_integration();
            }

            before = current;
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

        // time integrator
        ImGui::RadioButton("Euler", (int*)&body_.integration_, 0);
        ImGui::RadioButton("Midpoint", (int*)&body_.integration_, 1);
        ImGui::RadioButton("Verlet", (int*)&body_.integration_, 2);

        ImGui::Spacing();
        ImGui::Spacing();

        // time step
        ImGui::PushItemWidth(120);
        ImGui::SliderFloat("Time Step", &body_.time_step_, 0.00005f, 0.006f,
                           "%.5f", 2);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Forces", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::RadioButton("No Collisions", (int*)&body_.collisions_, 0);
        ImGui::RadioButton("Force-based Collisions", (int*)&body_.collisions_,
                           1);
        ImGui::RadioButton("Impulse-based Collisions", (int*)&body_.collisions_,
                           2);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Checkbox("Gravity", &body_.use_gravity_);

        ImGui::PushItemWidth(120);
        ImGui::SliderFloat("Damping", &body_.damping_, 0.0f, 1.0f, "%.2f", 1.5);
        ImGui::SliderFloat("Spring Stiffness", &body_.spring_stiffness_, 0.0f,
                           10000.0f, "%.0f", 3.5);
        ImGui::SliderFloat("Spring Damping", &body_.spring_damping_, 0.0f, 5.0f,
                           "%.2f", 1.5);
        ImGui::SliderFloat("Area Stiffness", &body_.area_stiffness_, 0.0f,
                           100000.0f, "%.0f", 3.5);
        ImGui::SliderFloat("Coll. Damping", &body_.collision_damping_, 0.0f,
                           1.0f, "%.2f", 1.5);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Spacing();
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
    int gui_width = 300, width_wo_gui = width() - gui_width;
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
    body_.draw(projection_matrix_);
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
    if (!body_.particles.empty())
    {
        // mouse button release destroys current mouse spring
        if (_button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_RELEASE)
        {
            body_.clear_mouse_spring();
        }

        // mouse button press generates new mouse spring
        else if (_button == GLFW_MOUSE_BUTTON_LEFT && _action == GLFW_PRESS)
        {
            double x, y;
            cursor_pos(x, y);
            body_.add_mouse_spring(pick(x, y));
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::motion(double _x, double _y)
{
    if (body_.is_mouse_spring_active())
    {
        body_.set_mouse_spring(pick(_x, _y));
    }
}

//=============================================================================
