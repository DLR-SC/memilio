add_library(sde_ui3
    infection_state.h
    model.h
    model.cpp
    parameters.h
    simulation.h
)
target_link_libraries(sde_ui3 PUBLIC memilio)
target_include_directories(sde_ui3 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_options(sde_ui3 PRIVATE ${MEMILIO_CXX_FLAGS_ENABLE_WARNING_ERRORS})
