file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

add_executable(mlmviewer ${SOURCES} ${HEADERS})
target_link_libraries(mlmviewer pmp_vis)

if (EMSCRIPTEN)
    set_target_properties(mlmviewer PROPERTIES LINK_FLAGS "--shell-file ${PROJECT_SOURCE_DIR}/external/pmp-library/src/apps/data/shell.html --preload-file ${PROJECT_SOURCE_DIR}/data@../data")
endif()
