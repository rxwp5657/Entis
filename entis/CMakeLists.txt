add_library(
    ${PROJECT_NAME} INTERFACE)

# Specify so IDEs can look at your files.
target_sources(
    ${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/sparse_set.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/registry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/component_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entis/error.h)

# It should use C++17.
target_compile_features(
    ${PROJECT_NAME}
    INTERFACE cxx_std_17)

# Add the .h files to the include path.
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
