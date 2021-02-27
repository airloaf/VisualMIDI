cmake_minimum_required(VERSION 3.9)

# Library locations (Change these to the appropriate path on your machine)
set(INCLUDE_DIR C:/DevelopmentLibraries/include/)
set(LIB_DIR C:/DevelopmentLibraries/lib/x64)

# Create a project for VSSynth
project(VisualMIDI_project)

file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
add_executable(VisualMIDI ${SRCS})

# Set include and link directories
target_include_directories(VisualMIDI PUBLIC ${INCLUDE_DIR})
target_link_directories(VisualMIDI PUBLIC ${LIB_DIR})

# Link Libraries
target_link_libraries(VisualMIDI SDL2)
target_link_libraries(VisualMIDI SDL2main)
target_link_libraries(VisualMIDI VSSynth)
target_link_libraries(VisualMIDI midifile)