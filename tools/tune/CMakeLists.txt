cmake_minimum_required(VERSION 3.16) 
project(Blocky VERSION 2.0.0)
set(CMAKE_CXX_STANDARD 20)
add_executable(extract extract.cpp)

target_sources(extract PRIVATE
    ../../src/moveOrder.cpp
    ../../src/moveGen.cpp
    ../../src/board.cpp
    ../../src/move.cpp
    ../../src/pieceSets.cpp
    ../../src/eval.cpp
    ../../src/attacks.cpp
    ../../src/bitboard.cpp
)
target_include_directories(extract PUBLIC "../../src/")
target_compile_options(extract PRIVATE -O3)
