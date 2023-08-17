# See https://cliutils.gitlab.io/modern-cmake/chapters/basics.html
# Require 3.25 due to SYSTEM in FetchContent_Declare calls.
cmake_minimum_required(VERSION 3.15)

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
set(DOWNLOAD_EXTRACT_TIMESTAMP ON)

include(FetchContent)

#FetchContent_Declare(RmlUi
#    GIT_REPOSITORY https://github.com/hobyst/RmlUi
#    GIT_TAG 104fdb35d3ea6320733bde72b38b1c4365ec49a5)
#FetchContent_MakeAvailable(RmlUi)

if (CMAKE_VERSION VERSION_LESS "3.25.0")
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
    #SYSTEM)
    )
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    #SYSTEM)
    )
FetchContent_MakeAvailable(SFML)

# We would use fetchcontent but nlohmann json is...
# it doesn't appear to have proper cmake setup for that.
# Not sure what's going on with that but we just download it.
#FetchContent_Declare(json
#    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
#    SYSTEM)
#FetchContent_MakeAvailable(json)
# Also note: https://github.com/nlohmann/json/pull/4080
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.2
    #SYSTEM)
    )
FetchContent_MakeAvailable(json)
else()
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
    SYSTEM)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# We would use fetchcontent but nlohmann json is...
# it doesn't appear to have proper cmake setup for that.
# Not sure what's going on with that but we just download it.
#FetchContent_Declare(json
#    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
#    SYSTEM)
#FetchContent_MakeAvailable(json)
# Also note: https://github.com/nlohmann/json/pull/4080
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.2
    SYSTEM)
FetchContent_MakeAvailable(json)
endif()

add_executable(
    AAToolCPP
    # Library things
    include/dmon.cpp include/dmon.hpp
    # Source things? Hmmm
    src/Overlay.cpp src/Overlay.hpp
    src/WindowManager.cpp src/WindowManager.hpp
    src/ResourceManager.cpp src/ResourceManager.hpp
    src/FileProvider.cpp src/FileProvider.hpp
    main.cpp)

if(${SANITIZE} STREQUAL "address")
    message("! Building with ASAN.")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

target_include_directories(AAToolCPP PRIVATE "include/")
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
target_link_libraries(AAToolCPP PRIVATE "-lpthread")
target_link_libraries(AAToolCPP PUBLIC sfml-graphics sfml-window sfml-system)
target_link_libraries(AAToolCPP PRIVATE fmt::fmt)
target_link_libraries(AAToolCPP PRIVATE nlohmann_json::nlohmann_json)

if (EXISTS ${PROJECT_BINARY_DIR}/compile_commands.json)
    file(COPY ${PROJECT_BINARY_DIR}/compile_commands.json DESTINATION ${PROJECT_SOURCE_DIR})
endif()
