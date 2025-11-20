file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_executable(rigid_bodies ${HEADERS} ${SOURCES})
target_link_libraries(rigid_bodies pmp stb_image)

if (EMSCRIPTEN)
    set_target_properties(rigid_bodies PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/../external/pmp/shell.html")
endif()
