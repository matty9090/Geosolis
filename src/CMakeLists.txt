file(GLOB_RECURSE GEO_SOURCES "*.cpp")
file(GLOB_RECURSE GEO_HEADERS "*.hpp")

foreach (_headerFile ${GEO_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND GEO_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES GEO_INCLUDE_DIRS)

add_executable(Geosolis ${GEO_SOURCES})
target_include_directories(Geosolis PRIVATE ${GEO_INCLUDE_DIRS})
target_link_libraries (Geosolis ${GEO_LIB_DIRS})

add_custom_command(
        TARGET Geosolis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/bin/Irrlicht.dll
                ${CMAKE_CURRENT_BINARY_DIR}/Irrlicht.dll)