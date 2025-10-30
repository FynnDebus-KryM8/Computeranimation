Computer Animation: Mass Spring Systems
=======================================

This package contains exercise code for the lecture "Computer Animation" by Prof. Dr. Mario Botsch, TU Dortmund.


Documentation
-------------

A pre-built HTML documentation can be found in `doc/index.html` and can be opened via any web browser.


Prerequisites
-------------

We use [CMake](www.cmake.org) for setting up build environments. You can download it [here](https://cmake.org/download/) or by using your favorite package manager.
You will also need a C++ compiler. Depending on your OS, the typical choices are:

  * Linux - GCC/G++, usually pre-installed (terminal command: `sudo apt install build-essential`)
  * MacOS - Clang, comes with the IDE "XCode" (terminal command: `xcode-select --install`)
  * Windows - MSVC, can be installed alongside the IDE "Visual Studio Community" (see below) or separately [here](https://visualstudio.microsoft.com/downloads/#other) under "All Downloads" --> "Tools for Visual Studio" --> "Build Tools for Visual Studio 2019"

We highly recommend to use some kind of IDE. Prominent examples are:
 
 - [XCode](https://developer.apple.com/xcode/) (MacOS) 
 - [Visual Studio Community](https://visualstudio.microsoft.com/de/vs/community/) (Windows)
 - [Visual Studio Code](https://code.visualstudio.com/) (Linux, Windows, MacOS)
 - [Jetbrains CLion](https://www.jetbrains.com/de-de/clion/) (Linux, Windows, MacOS)
 
Below, we provide examples for setting up, compiling and running the project via command line, XCode, VSCommunity and VSCode.


Building on Linux via Command Line (no IDE)
-------------------------------------------

Depending on your system, you might have to install some libraries for OpenGL development. On Ubuntu systems, this would do the job:

    sudo apt-get install build-essential libglu1-mesa-dev libgl-dev libx11-dev xorg-dev libxmu-dev libxi-dev 
 
Execute the following commands in the exercise's top-level directory:

    mkdir build
    cd build
    cmake ..
    make

The last command (`make`) compiles the application. Re-run it whenever you have added/changed code in order to re-compile.

For running the code via command line use

    ./mass_springs


Building on MacOS (XCode)
--------------------------

Execute the following commands in the exercise's top-level directory:

    mkdir xcode
    cd xcode
    cmake -G Xcode ..
    open mass_springs.xcodeproj

Within XCode, select the target executable `mass_springs` in the top bar. You can compile and run the code by pressing the play button. You can specify command line arguments by again opening the menu next to the stop button from the top bar and selecting "Edit Scheme". In the popup window, select the "Run" option left and open the "Arguments" tab.


Building on Windows (Visual Studio Community) 
---------------------------------------------

* In order to get the MSVC C++ compiler, make sure that you check "Desktop development with C++" during installation of [VSCommunity](https://visualstudio.microsoft.com/de/vs/community/)
* Create an empty build folder inside the project's top-level directory
* Start cmake-gui.exe (located in cmake's bin folder)
* Specify the top-level directory as source directory (button Browse source...)
* Specify the previously created build folder as build directory (button Browse build...)
* Select "Configure" using your Visual Studio Version as option.
* When configuration is finished, select "Generate".
* Start Visual Studio Community
* Open the project via File -> open -> project -> .sln in build folder
* In the project explorer window on the right, right-click the project (mass-springs) and set it as startup-project
* Switch to release mode
* You can specify command line arguments via project -> properties -> debugging -> command arguments
* Hit CTRL + F5 to build and run (or CTRL + SHIFT + B to build)


Building via VSCode
-------------------

There are a lot of useful extensions for VSCode to shape it the way you like.
The required extensions for C++ development and Cmake support are "C/C++" and "Cmake Tools". Extensions can be found in the extensions tab on the left.
Once this is done, you can set up the project:

 * Start VSCode
 * Open the project via File --> Open Folder and select the exercise's top-level directory containing the `CMakeLists.txt` file and accept by clicking `OK`
 * In the bottom bar, click on `CMake`, choose your compiler and select `Release` mode
 * In the bottom bar, select `[mass_springs]` as default build target.
 * Still in the bottom bar, there are buttons for building (Build) and running (play symbol)
 * You can specify command line arguments by using the terminal inside of VSCode

Have a look at the VSCode [documentation](https://code.visualstudio.com/docs/cpp/introvideos-cpp) for further details. 



Running
-------

When running the programm, you can interact by using the following keys:

* Press the buttons `1` to `4` to load different objects.
* Press `spacebar` to start/stop the animation.
* Press `backspace` to reset simulation parameters.
* Press `s` to perform a single time-step.
* Press `g` to show/hide the GUI.

Using the buttons and sliders in the GUI, you can do a lot more fancy stuff.


Todo
----

In `src/MassSpringSystem.cpp` you will find some points marked with `ToDo`. You can find those in the [ToDo-List](todo.html):

1. **Force calculations**:
First we implement the different force calculations. You can tackle them one by one, and after each step compile the program and check your progress. To help you with the area forces, take a look at how a triangle's area is calculated in `Triangle.h`. For the spring connecting one particle to the mouse pointer, use the attributes of `mouse_spring_` to get both the pointer's position as well as the particle that is closest to it -- calculated in `void Viewer::mouse(...)`. In general, look what the classes `Particle`, `Spring`, and `Triangle` store and compute.

2. **Midpoint integration** and **Velocity Verlet integration**:
Euler integration has already been implemented. Use it as a starting point for implementing Midpoint integration and Velocity Verlet integration, which mostly involves looking up the equations on the lecture slides and transferring them to C++ code.

3. **Impulse-based collision handling**:
To tackle collisions, both impulse-based and force-based, you need to check for collisions with each of the four planes. As can be seen in the function `MassSpringSystem::draw()`, the drawn square lies between the points `(-1.0, -1.0)` and `(1.0, 1.0)`, i.e. the two horizontal lines have `y`-coordinates `-1.0` and `1.0`, whereas the two vertical lines have `x`-coordinates `-1.0` and `1.0`.

Fill in the missing code, compile, and enjoy.



License
-------

The code examples are copyright Computer Graphics Group, TU Dortmund.

Have fun!
