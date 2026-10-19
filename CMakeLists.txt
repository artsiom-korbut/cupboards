cmake_minimum_required(VERSION 3.22)

project(
    Cupboards
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.18
)
FetchContent_Declare(
    SDL_TTF
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
)

set(SDLTTF_VENDORED ON)
FetchContent_MakeAvailable(SDL SDL_TTF)

file(GLOB_RECURSE CUPBOARD_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(Cupboards ${CUPBOARD_SOURCES})
target_include_directories(Cupboards PRIVATE src)
target_compile_features(Cupboards PRIVATE cxx_std_17)
target_link_libraries(Cupboards PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
