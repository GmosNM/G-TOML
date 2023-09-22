cmake_minimum_required(VERSION 3.12)
project(GTOML)

set(CMAKE_CXX_STANDARD 17)



# Test
set(SRC
    tests/main.cpp
)


add_executable(tests ${SRC})

target_link_libraries(tests libgtoml)

# Add all the source files from the G-TOML project
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the static library
add_library(libgtoml STATIC ${SOURCES})

# Include the headers from the G-TOML project
target_include_directories(libgtoml PUBLIC "src")


install(TARGETS libgtoml DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/GTOML FILES_MATCHING PATTERN "*.hpp")


