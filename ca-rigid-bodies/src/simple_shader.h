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

static const char* phong_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
    "\n"
    "layout (location=0) in vec4 v_position;\n"
    "layout (location=1) in vec3 v_normal;\n"
    "out vec3 v2f_normal;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   v2f_normal  = v_normal;\n"
    "   gl_Position = modelview_projection_matrix * v_position;\n"
    "} \n";

static const char* phong_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
    "precision mediump float;\n"
#endif
    "in vec3  v2f_normal;\n"
    "uniform bool   use_lighting;\n"
    "uniform vec3   color;\n"
    "float  ambient   = 0.1;\n"
    "float  diffuse   = 0.8;\n"
    "float  specular  = 1.0;\n"
    "float  shininess = 100.0;\n"
    "vec3   light1    = vec3( 1.0, 1.0, 1.0);\n"
    "vec3   light2    = vec3(-1.0, 1.0, 1.0);\n"
    "out vec4 f_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 rgb;\n"
    "\n"
    "    if (use_lighting)\n"
    "    {\n"
    "       vec3 L1 = normalize(light1);\n"
    "       vec3 L2 = normalize(light2);\n"
    "       vec3 N  = normalize(v2f_normal);\n"
    "       vec3 V  = vec3(0,0,1);\n"
    "       \n"
    "       vec3  R;\n"
    "       float NL, RV;\n"
    "       \n"
    "       rgb = ambient * 0.1 * color;\n"
    "       \n"
    "       NL = dot(N, L1);\n"
    "       if (NL > 0.0)\n"
    "       {\n"
    "           rgb += diffuse * NL * color;\n"
    "           R  = normalize(-reflect(L1, N));\n"
    "           RV = dot(R, V);\n"
    "           if (RV > 0.0) \n"
    "           {\n"
    "               rgb += vec3( specular * pow(RV, shininess) );\n"
    "           }\n"
    "       }\n"
    "       \n"
    "       NL = dot(N, L2);\n"
    "       if (NL > 0.0)\n"
    "       {\n"
    "            rgb += diffuse * NL * color;\n"
    "            R  = normalize(-reflect(L2, N));\n"
    "            RV = dot(R, V);\n"
    "            if (RV > 0.0) \n"
    "            {\n"
    "                rgb += vec3( specular * pow(RV, shininess) );\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "   \n"
    "    // do not use lighting\n"
    "    else\n"
    "    {\n"
    "        rgb = color;\n"
    "    }\n"
    "    \n"
    "   \n"
    "    f_color = vec4(rgb, 1.0);\n"
    "}";

//=============================================================================
