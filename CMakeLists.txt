cmake_minimum_required (VERSION 2.6)
project (Geosolis)

set (GEO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/inc/")
set (GEO_LIBS "${PROJECT_SOURCE_DIR}/lib/Irrlicht.lib ${PROJECT_SOURCE_DIR}/lib/libnoise.lib")

add_subdirectory(src)