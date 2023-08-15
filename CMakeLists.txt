# See https://cliutils.gitlab.io/modern-cmake/chapters/basics.html
cmake_minimum_required(VERSION 3.14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # We need this lol.
option(SANITIZE "Whether or not to sanitize builds" "NO_SANITIZE")

project(
    AAToolCPP
    VERSION 0.1
    DESCRIPTION "AA Tool, C++, Bad"
    LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD
    20
    CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

#FetchContent_Declare(RmlUi
#    GIT_REPOSITORY https://github.com/hobyst/RmlUi
#    GIT_TAG 104fdb35d3ea6320733bde72b38b1c4365ec49a5)
#FetchContent_MakeAvailable(RmlUi)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(
    AAToolCPP
    src/Overlay.cpp src/Overlay.hpp
    src/WindowManager.cpp src/WindowManager.hpp
    src/ResourceManager.cpp src/ResourceManager.hpp
    main.cpp)

if(${SANITIZE} STREQUAL "address")
    message("! Building with ASAN.")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

# target_include_directories(AAToolCPP PRIVATE "src/")
#target_link_libraries(AAToolCPP PRIVATE
#    # RmlUi::RmlUi
#    )

if(APPLE)
    # Required for dmon
    target_link_libraries(AAToolCPP PRIVATE
        "-framework CoreFoundation"
        "-framework CoreServices"
    )
endif()
target_link_libraries(AAToolCPP PUBLIC sfml-graphics sfml-window sfml-system)
target_link_libraries(AAToolCPP PRIVATE fmt::fmt)
target_link_libraries(AAToolCPP PRIVATE nlohmann_json::nlohmann_json)

file(COPY ${PROJECT_BINARY_DIR}/compile_commands.json DESTINATION ${PROJECT_SOURCE_DIR})
