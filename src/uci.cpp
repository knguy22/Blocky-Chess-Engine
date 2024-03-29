/*
* Blocky, a UCI chess engine
* Copyright (C) 2023-2024, Kevin Nguyen
*
* Blocky is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* Blocky is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program;
* if not, see <https://www.gnu.org/licenses>.
*/

#include <cctype>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include "uci.hpp"
#include "perft.hpp"
#include "bench.hpp"
#include "timeman.hpp"
#include "ttable.hpp"
#include "search.hpp"
#include "moveOrder.hpp"
#include "moveGen.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include "utils/types.hpp"

namespace Uci {

UciOptions OPTIONS;

void printEngineInfo() {
    std::string input;
    while (input != "uci") {
        std::getline(std::cin, input);
    }

    std::cout << "id name Blocky V2.0\n";
    std::cout << "id author knguy22/intermittence, aqiu04\n";

    std::cout << "option name maxDepth type spin default 100 min 1 max 200\n";
    std::cout << "option name Hash type spin default 128 min 128 max 1024\n";

    std::cout << "uciok\n";
}

void setOptionLoop() {
    while (true) {
        std::string commandLine, commandToken;
        std::getline(std::cin, commandLine);
        std::istringstream commandStream(commandLine);
        commandStream >> commandToken;

        if (commandToken == "setoption") {setOption(commandStream);}
        else if (commandToken == "isready") {isready(); break;}
    }
}

void uciLoop() {
    Board currBoard;
    while (true) {
        std::string commandLine, commandToken;
        std::getline(std::cin, commandLine);
        std::istringstream commandStream(commandLine);
        commandStream >> commandToken;

        if (commandToken == "ucinewgame") {uciNewGame();}
        else if (commandToken == "position") {currBoard = position(commandStream);}
        else if (commandToken == "go") {Uci::go(commandStream, currBoard);}
        else if (commandToken == "isready") {isready();}
        else if (commandToken == "bench") {bench();}
        else if (commandToken == "perft") {perft(commandStream, currBoard);}
        else if (commandToken == "magics") {magics();}
        else if (commandToken == "quit") {return;}
    }
}

void setOption(std::istringstream& input){ 
    // Example: setoption name maxDepth value 2

    // gather inputs
    std::string token, id, value;
    input >> token; // name qualifier
    input >> id;
    input >> token; // value qualifier
    input >> value;

    // uci requires id to not be case sensitive
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);

    if (id == "maxdepth") {
        OPTIONS.depth = std::stoi(value);
    }
    else if (id == "hash") {
        TTable::Table.resize(std::stoi(value));
    }
}

void uciNewGame() {
    TTable::Table.clear();
}

Board position(std::istringstream& input) {
    std::string token;
    Board currBoard;

    input >> token;
    if (token == "startpos") {
        input >> token;
        currBoard = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    else if (token == "fen") {
        std::string fenStr;
        while (input >> token && token != "moves") { 
            fenStr += token + ' ';
        }
        currBoard = Board(fenStr);
    }

    if (token != "moves") {return currBoard;}
    while (input >> token) {
        currBoard.makeMove(Move(token, currBoard.isWhiteTurn()));
        // if a capture or castling rights change, clear move history since
        // 3fold repetition or 50 move rule will be reset
        // this makes things faster
        if (currBoard.lastMoveCaptureOrCastle()) {
            currBoard.clearHistory();
        }
    }
    return currBoard;
}

void go(std::istringstream& input, Board& board) {
    std::string token;

    // initialize time parameters
    int wtime, btime, winc, binc;
    wtime = btime = Timeman::INF_TIME;
    winc = binc = 0;

    // input time parameters
    std::string param, value;
    while (input >> param) {
        input >> value;
        if (param == "wtime") {wtime = std::stoi(value);}
        else if (param == "btime") {btime = std::stoi(value);}
        else if (param == "winc") {winc = std::stoi(value);}
        else if (param == "binc") {binc = std::stoi(value);}
    }
    const int allytime = board.isWhiteTurn() ? wtime : btime;
    const int allyInc = board.isWhiteTurn() ? winc : binc;
    Timeman::TimeManager tm(allytime, allyInc);

    // begin search
    Search::Searcher currSearch(board, tm, OPTIONS.depth);
    Search::Info result = currSearch.startThinking();
    std::cout << "bestmove " << result.move.toStr() << "\n";
}

void isready() {
    std::cout << "readyok\n";
}

void bench() {
    uciNewGame(); // required to make benches consistent
    const uint64_t result = Bench::start();
    std::cout << "Bench results: " << result << '\n';
}

void perft(std::istringstream& input, Board& board) {
    // validate arguments
    std::string token;
    int depth;
    if (!(input >> token)) {
        std::cout << "ARGUMENT ERROR: Perft requires a depth to search to" << std::endl;
        return;
    }
    try {
        depth = std::stoi(token);
    } 
    catch(std::exception& e) {
        std::cout << "ARGUMENT ERROR: Perft requires an integer to search to" << std::endl;
        return;
    }
    
    // perform perft
    const auto start = std::chrono::high_resolution_clock::now();
    const uint64_t nodes = perft<true>(board, depth);
    const auto end = std::chrono::high_resolution_clock::now();
    const int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "perft result nodes " << nodes;
    std::cout << " nps " << nodes * 1000000 / duration;
    std::cout << " time " << duration / 1000 << "\n";
}

void magics() {
    Attacks::generateMagics();
}

} // namespace Uci

