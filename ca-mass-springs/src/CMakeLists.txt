file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)

add_executable(mass_springs ${HEADERS} ${SOURCES})
target_link_libraries(mass_springs pmp stb_image)

if (EMSCRIPTEN)
    set_target_properties(mass_springs PROPERTIES LINK_FLAGS "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/../external/pmp/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data/@./data/")
endif()
