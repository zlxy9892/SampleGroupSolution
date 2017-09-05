cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(SampleGroup_Project)

set(GDAL_INCLUDE_DIR D:/dev/gdal64/include)
set(GDAL_LIB_DIR D:/dev/gdal64/lib)
set(GDAL_LIBS gdal_i.lib)

include_directories(${GDAL_INCLUDE_DIR})
link_directories(${GDAL_LIB_DIR})

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
source_group("Header Files" FILES ${CURRENT_HEADERS})

aux_source_directory(. DIR_SRCS)

add_executable (SampleGroup ${DIR_SRCS} ${CURRENT_HEADERS})
target_link_libraries (SampleGroup ${GDAL_LIBS})