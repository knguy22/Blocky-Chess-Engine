# Blocky Chess Engine

## Overview

Blocky is a UCI-compliant chess engine. It has implemented the following techniques below thus far: 

* Search:
    * Alpha-Beta Search
        * Aspiration Windows
        * Principle Variation Search
    * Quiescent Search
    * Null Move Pruning
    * Reverse Futility Pruning
    * Internal Iterative Reductions
    * Late Move Reductions
    * Late Move Pruning
    * Transposition Tables Cutoffs
* Evaluation: 
    * Piece-Square Tables
    * Tapered Evaluation
    * Pieces
        * Mobility (Bishop, Knight, Rook)
        * Bishop Pair
    * Pawns
        * Passed Pawns
        * Doubled Pawns
        * Chained Pawns
        * Phalanx Pawns
* Move Ordering:
    * Transposition Table Moves
    * MVV-LVA
    * Killer Move Heuristic
    * Butterfly History Heuristic
    * Staged Move Generation
* Other Techniques:
    * Magic Bitboards and Attack Tables
    * Zobrist-Hashing

## Compiling Blocky

In order to get started running Blocky, first clone this repository. Then, use CMake to build this project. CMake projects like this one can be built like so:

```
cmake -S . -B build
cmake --build build
```

As part of the development process used for Blocky, Windows binaries may be compiled from WSL using the modified command below:

```
cmake -S . -B build -DMAKE_EXE=ON
cmake --build build
```

In both cases, the binary will be located within the ```build``` folder. 

## Acknowledgements

This engine would have not been possible alone. Some resources used included but were not limited to:
* The Chess Programming Wiki for providing a foundation on how to get started
* The chess engine community's collective knowledge
* Reference engines such as Stockfish, Ethereal, Smallbrain, Stormphrax, and Alexandria
* Gedas for his Texel-Tuner used in Blocky's evaluation
