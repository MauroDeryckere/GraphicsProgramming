include(FetchContent)

# add gtest
FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)


# add source files
set(SOURCES 
    "../src/Matrix.cpp"
    "../src/Renderer.cpp"
    "../src/Scene.cpp"
    "../src/Timer.cpp"
    "../src/Vector3.cpp"
    "../src/Vector4.cpp"
)

# add test source files
set(TESTS
    "UnitTests.cpp"
)


# include SDL because camera class needs it
set(SDL_DIR "${CMAKE_SOURCE_DIR}/project/libs/SDL2-2.30.3")
add_library(SDL STATIC IMPORTED)
set_target_properties(SDL PROPERTIES
    IMPORTED_LOCATION "${SDL_DIR}/lib/SDL2.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_DIR}/include"
)


add_executable(UnitTests ${SOURCES} ${TESTS})
target_link_libraries(UnitTests gtest gtest_main SDL)

# only needed if header files are not in same directory as source files
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add unit tests for test runner to discover
add_test(NAME UnitTests COMMAND UnitTests)
