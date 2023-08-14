cmake_minimum_required(VERSION 3.14)
project(BLOCKY-CHESS-ENGINE)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()
add_executable(
    allTests
    tests/testBoard.cpp
    tests/testInCheck.cpp
    src/board.cpp
    src/inCheck.cpp
    src/moveGen.cpp
)
target_include_directories(
    allTests PUBLIC "src/"
)
target_link_libraries(
    allTests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(allTests)