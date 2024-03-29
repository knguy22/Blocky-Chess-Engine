
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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "search.hpp"
#include "ttable.hpp"
#include "moveOrder.hpp"
#include "eval.hpp"
#include "moveGen.hpp"
#include "board.hpp"
#include "timeman.hpp"
#include "zobrist.hpp"
#include "utils/fixedVector.hpp"

namespace Search {

std::array<std::array<int, MAX_MOVES>, MAX_PLY> LMRTable{};
void initLMRTable() {
    for (int depth = 1; depth < MAX_PLY; ++depth) {
        for (int moves = 1; moves < MAX_MOVES; ++moves) {
            LMRTable[depth][moves] = static_cast<int>(1.2 + std::log(depth) * std::log(moves) / 4.00);
        }
    }
}

Info Searcher::startThinking() {
    Info result;

    // stack needs to label distances from root
    for (size_t i = 0; i < this->stack.size(); ++i) {
        this->stack[i].ply = i;
    }

    // perform iterative deepening
    int prevEval = NO_SCORE;
    for (int i = 1; i <= this->depth_limit; i++) {
        const int score = this->aspiration(i, prevEval);
        prevEval = score;
        result.move = this->PVTable[0].moves[0];
        result.nodes = this->nodes;
        result.timeElapsed = this->tm.getTimeElapsed();

        // if it's not possible to search deeper, stop searching 
        if (this->max_seldepth < i) {
            break;
        } else {
            result.depth = i;
            result.seldepth = this->max_seldepth;
        }

        // only update eval for completed searches
        if (!this->stopSearching()) {
            result.eval = score;
        }

        // compute mate-in
        if (abs(result.eval) >= MATE_IN_SCORE) {
            int playerMating = result.eval < 0 ? -1 : 1;
            result.mateIn = playerMating * (INF_SCORE - abs(result.eval)); // convert eval to ply
            result.mateIn = (result.mateIn + playerMating) / 2; // convert ply to moves
        }

        if (this->printInfo) {
            this->outputUciInfo(result);
        }
        
        // break out of search early for optimistic time used; this also applies to hard time up
        if (this->tm.softTimeUp()) {
            break;
        }
    }

    return result;
}

int Searcher::aspiration(int depth, int prevEval) {
    int delta = 40;
    int alpha, beta;

    // don't use aspiration bounds for low search depths as scores at low depths are less stable
    if (depth <= 6) {
        alpha = -INF_SCORE;
        beta = INF_SCORE;
    } else {
        alpha = prevEval - delta;
        beta = prevEval + delta;
    }

    // search until an exact score has been found with the aspiration bounds or search has been stopped
    int result;
    while (true) {
        result = this->search<ROOT>(alpha, beta, depth, &this->stack[0]);

        if (this->stopSearching() || (alpha < result && result < beta)) {
            break;
        }

        alpha = std::max(alpha - delta, -INF_SCORE);
        beta = std::min(beta + delta, INF_SCORE);
        delta *= 2;
    }
    return result;
}

template <NodeTypes NODE>
int Searcher::search(int alpha, int beta, int depth, StackEntry* ss) {
    constexpr bool ISROOT = NODE == ROOT;
    constexpr bool ISPV = NODE == ROOT || NODE == PV;
    constexpr bool ISNMP = NODE == NMP;
    const int oldAlpha = alpha;
    this->PVTable[ss->ply].length = ss->ply;

    // time up
    if (this->stopSearching()) {
        return NO_SCORE;
    }

    ++this->nodes;
    this->max_seldepth = std::max(ss->ply, this->max_seldepth);

    if (this->board.isDraw()) {
        return DRAW_SCORE;
    }
    // max depth reached
    if (depth <= 0) {
        return quiesce(alpha, beta, ss);
    }

    /************
     * Probe Tranposition Table
    *************/
    Move TTMove;
    int staticEval;
    if (TTable::Table.entryExists(this->board.zobristKey())) {
        const TTable::Entry entry = TTable::Table.getEntry(this->board.zobristKey());

        if (!ISPV && entry.depth >= depth) {
            if (entry.bound == EvalType::EXACT
                || (entry.bound == EvalType::UPPER && entry.eval <= alpha)
                || (entry.bound == EvalType::LOWER && entry.eval >= beta)) {
                return entry.eval;
            }
        }

        TTMove = entry.move;
        staticEval = entry.eval;
    } else {
        staticEval = this->board.evaluate();
    }

    /************
     * Internal Iterative Reductions
     * Nodes that don't have a TTMove are less likely to be important
    *************/
    if (!ISROOT && !TTMove && depth >= 6) {
        depth--;
    }

    /************
     * Reverse Futility Pruning
     * If the evaluation is too far above beta, assume that there is no chance for the opponent to catch up
    *************/
    if (!ISPV && depth < 5 && staticEval - (100 * depth) >= beta) {
        return beta;
    }

    const bool inCheck = currKingInAttack(this->board.pieceSets, this->board.isWhiteTurn());
    /************
     * Null Move Pruning
     * Give the opponent a free move and see if our position is still too good after that; if so, prune
     * This assumes that the opponent cannot improve their position by much given our reduced depth
    *************/
    if (!ISNMP
        && !inCheck
        && depth >= 2
        && staticEval >= beta
        && this->board.hasNonPawnMat()) {

        // prefetch TT entry as soon as possible; NMP only changes color
        TTable::Table.prefetch(this->board.zobristKey() ^ Zobrist::isBlackKey);

        int reduction = 3 + depth / 4;
        board.makeNullMove();
        int nullMoveScore = -search<NMP>(-beta, -beta + 1, depth - reduction, ss + 1);
        board.unmakeNullMove();
        if (nullMoveScore >= beta) {
            // don't return mated stores
            if (nullMoveScore >= MATE_IN_SCORE) {
                nullMoveScore = beta;
            }
            return nullMoveScore;
        }
    }

    // init movePicker
    MoveOrder::MovePicker movePicker(this->board, this->history, MoveOrder::All, TTMove, ss->killerMove);

    // start search through moves
    int score = NO_SCORE, bestscore = -INF_SCORE;
    Move bestMove{};
    FixedVector<Move, MAX_MOVES> failedQuiets{};
    bool doFullNullSearch, doPVS, skipQuiets = false;

    while (movePicker.movesLeft(this->board, this->history)) {
        const Move move = movePicker.pickMove();
        const bool quietMove = !movePicker.stagesLeft();

        /*************
         * Late Move Pruning:
         * Moves that are searched later are less likely to be good
         * Still search captures though
        **************/
        if (!skipQuiets) {
            if (!ISPV
                && movePicker.getMovesPicked() >= 30
                && !inCheck) {

                skipQuiets = true;
            }
        }
        if (skipQuiets && quietMove) {
            continue;
        }

        board.makeMove(move);
        // prefetch TT entry as soon as possible
        TTable::Table.prefetch(this->board.zobristKey());
        const bool moveGivesCheck = currKingInAttack(this->board.pieceSets, this->board.isWhiteTurn());

        /*************
         * Extensions:
        **************/
        int extensions = 0;
        extensions += static_cast<int>(moveGivesCheck);
        int newDepth = depth + extensions - 1;

        /*************
         * Late Move Reductions (LMR):
         * Search moves that are likely to be less good to lower depths with null bounds
         * Researches will happen with LMR fails
        **************/
        if (quietMove
            && movePicker.getMovesPicked() >= 4 
            && depth >= 3
            && !moveGivesCheck) {
            
            int reduction = LMRTable[depth][movePicker.getMovesPicked()];
            int LMRDepth = newDepth - reduction;

            score = -search<NOTPV>(-alpha - 1, -alpha, LMRDepth, ss + 1);
            doFullNullSearch = score > alpha && LMRDepth < newDepth;
        } else {
            doFullNullSearch = !ISPV || movePicker.getMovesPicked() > 1;
        }

        if (doFullNullSearch) {
            score = -search<NOTPV>(-alpha - 1, -alpha, newDepth, ss + 1);
        }
        /*************
         * Principle Variation Search (PVS):
         * Search with full bounds with null bounds fail
        **************/
        doPVS = ISPV && ((score > alpha && score < beta) || movePicker.getMovesPicked() == 1);
        if (doPVS) {
            score = -search<PV>(-beta, -alpha, newDepth, ss + 1);
        }
        board.undoMove(); 

        // don't update best move if time is up
        if (this->stopSearching()) {
            return bestscore;
        }
        
        // fail-soft stabilizes the search and allows for returned values outside the alpha-beta bounds
        if (score > bestscore) {
            bestscore = score;
            bestMove = move;

            // update the PV-Table
            if (ISPV) {
                this->PVTable[ss->ply].moves[ss->ply] = move;
                for (int i = ss->ply + 1; i < this->PVTable[ss->ply + 1].length; ++i) {
                    this->PVTable[ss->ply].moves[i] = this->PVTable[ss->ply + 1].moves[i];
                }
                this->PVTable[ss->ply].length = this->PVTable[ss->ply + 1].length;
            }

            // update alpha if we have proven that we can guarantee that lower bound
            if (score > alpha) {
                alpha = score;

                // prune if a move is too good, opponent will avoid playing into this node
                if (score >= beta) {
                    // updating history and killer moves orders then ahead of other moves
                    this->history[move.sqr1()][move.sqr2()] += depth * (depth - 1);
                    if (quietMove) {
                        ss->killerMove = move;

                        // apply malus for quiets that didn't cause beta cutoffs
                        // these quiets were ordered ahead of the cutting move, so they should be penalized
                        for (const auto& quiet: failedQuiets) {
                            this->history[quiet.sqr1()][quiet.sqr2()] -= depth * (depth - 1);
                        }
                    }
                    break;
                }
            }
        }

        // keep track of all quiets that didn't generate cutoffs
        if (quietMove) {
            failedQuiets.push_back(move);
        }
    }

    // checkmate or stalemate
    if (movePicker.getMovesPicked() == 0) {
        if (inCheck) {
            return -INF_SCORE + ss->ply;
        }
        return DRAW_SCORE;
    }

    // store results with best moves in transposition table
    if (bestMove) {
        const EvalType bound = (bestscore >= beta) ? EvalType::LOWER : (alpha == oldAlpha) ? EvalType::UPPER : EvalType::EXACT;
        TTable::Table.store(bestscore, bestMove, bound, depth, this->board.zobristKey());
    }
    return bestscore;
}

int Searcher::quiesce(int alpha, int beta, StackEntry* ss) {
    if (this->stopSearching()) {
        return NO_SCORE;
    }

    ++this->nodes;
    this->max_seldepth = std::max(ss->ply, this->max_seldepth);

    const int stand_pat = this->board.evaluate();
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    MoveOrder::MovePicker movePicker(this->board, this->history, MoveOrder::Captures);
    int score = -INF_SCORE;
    while (movePicker.movesLeft(this->board, this->history)) {
        Move move = movePicker.pickMove();
        board.makeMove(move);
        score = -quiesce(-beta, -alpha, ss + 1);
        board.undoMove(); 

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;

}


bool Searcher::stopSearching() {
    // only check system time every 1024 nodes for performance
    if (this->nodes % 1024 == 0 && !this->stopSearchFlag) {
        this->stopSearchFlag = this->tm.hardTimeUp();
    }
    return this->stopSearchFlag;
}

void Searcher::outputUciInfo(Info searchResult) const {
    std::cout << "info depth " << searchResult.depth << ' ';
    std::cout << "seldepth " << searchResult.seldepth << ' ';
    std::cout << "nodes " << searchResult.nodes << ' ';

    // time is output in milliseconds per the UCI protocol
    std::cout << "time " << searchResult.timeElapsed << ' ';
    if (searchResult.timeElapsed > 0) { // prevents divide by 0
        std::cout << "nps " << searchResult.nodes * 1000 / searchResult.timeElapsed  << ' ';
    }
    
    if (searchResult.mateIn == NO_SCORE) {
        std::cout << "score cp " << searchResult.eval << ' ';
    } else {
        std::cout << "score mate " << searchResult.mateIn << ' ';
    }
    std::cout << "hashfull " << TTable::Table.hashFull() << ' ';

    // principle variations are checked for a valid sequence of moves; if not valid, a warning is given;
    std::cout << "pv ";
    Board tmpBoard = this->board;
    Move move;
    bool illegalMove = false;
    for (int i = 0; i < this->PVTable[0].length; ++i) {
        move = this->PVTable[0].moves[i];
        std::cout << move.toStr() << ' ';
        if (!tmpBoard.isLegalMove(move)) {
            illegalMove = true;
            break;
        }

        tmpBoard.makeMove(move);
    }
    std::cout << std::endl;

    if (illegalMove) {
        std::cout << "Warning! illegal move in PV: " << move.toStr() << std::endl;
    }
}

} // namespace Search
