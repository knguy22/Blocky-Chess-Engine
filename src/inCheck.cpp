#include "inCheck.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

bool isFriendlyPiece(Board& currBoard, BoardSquare targetSquare) {
    int target = currBoard.getPiece(targetSquare);
    if(currBoard.isWhiteTurn) {
        return target >= WKing && target <= WRookUnmoved;
    }
    else {
        return target >= BKing && target <= BRookUnmoved;
    }
}


pieceTypes getPieceInDirection(Board& currBoard, BoardSquare originSquare, int rankIncrement, int fileIncrement) {
    if (rankIncrement == 0 && fileIncrement == 0) {
        throw std::invalid_argument("rankIncrement or fileIncrement must not be 0");
    }
    
    pieceTypes currPiece;
    int currRank = originSquare.rank + rankIncrement;
    int currFile = originSquare.file + fileIncrement;

    while(currRank >= 0 && currRank <= 7 && currFile >= A && currFile <= H) {
        currPiece = currBoard.getPiece(currRank, currFile);
        if (currPiece != EmptyPiece) {
            return currPiece;
        }
        currRank += rankIncrement;
        currFile += fileIncrement;
    }
    return nullPiece;
}

bool checkDiagAttackers(Board& currBoard, BoardSquare originSquare, pieceTypes originPiece) {
    std::vector<pieceTypes> possibleAttackers;
    if (currBoard.isWhiteTurn) {
        possibleAttackers.push_back(BQueen);
        possibleAttackers.push_back(BBishop);
    }
    else {
        possibleAttackers.push_back(WQueen);
        possibleAttackers.push_back(WBishop);
    }

    std::vector<pieceTypes> cornerPieces; 
    cornerPieces.push_back(getPieceInDirection(currBoard, originSquare, 1, -1)); // down left
    cornerPieces.push_back(getPieceInDirection(currBoard, originSquare, 1, 1)); // down right
    cornerPieces.push_back(getPieceInDirection(currBoard, originSquare, -1, -1)); // up left
    cornerPieces.push_back(getPieceInDirection(currBoard, originSquare, -1, 1)); // up right
    
    for (pieceTypes piece: cornerPieces) {
        if (std::find(possibleAttackers.begin(), possibleAttackers.end(), piece) != possibleAttackers.end()) {
            return true;
        }
    }
    return false;
}

bool checkStraightAttackers(Board& currBoard, BoardSquare originSquare, pieceTypes originPiece) {
    std::vector<pieceTypes> possibleAttackers;
    if (currBoard.isWhiteTurn) {
        possibleAttackers.push_back(BQueen);
        possibleAttackers.push_back(BRook);
        possibleAttackers.push_back(BRookUnmoved);
    }
    else {
        possibleAttackers.push_back(WQueen);
        possibleAttackers.push_back(WRook);
        possibleAttackers.push_back(WRookUnmoved);
    }

    std::vector<pieceTypes> crossPieces; 
    crossPieces.push_back(getPieceInDirection(currBoard, originSquare, 1, 0)); // down
    crossPieces.push_back(getPieceInDirection(currBoard, originSquare, -1, 0)); // up
    crossPieces.push_back(getPieceInDirection(currBoard, originSquare, 0, -1)); // left
    crossPieces.push_back(getPieceInDirection(currBoard, originSquare, 0, 1)); // right
    
    for (pieceTypes piece: crossPieces) {
        if (std::find(possibleAttackers.begin(), possibleAttackers.end(), piece) != possibleAttackers.end()) {
            return true;
        }
    }
    return false;
}



bool checkKnightAttackers(Board& currBoard, BoardSquare originSquare, pieceTypes originPiece) {
    pieceTypes enemyKnight = currBoard.isWhiteTurn ? BKnight : WKnight;

    std::vector<pieceTypes> knightSquares; 
    int rank = originSquare.rank;
    int file = originSquare.file;
    knightSquares.push_back(currBoard.getPiece(rank - 2, file - 1));
    knightSquares.push_back(currBoard.getPiece(rank - 2, file + 1));
    knightSquares.push_back(currBoard.getPiece(rank + 2, file - 1));
    knightSquares.push_back(currBoard.getPiece(rank + 2, file + 1));
    knightSquares.push_back(currBoard.getPiece(rank - 1, file - 2));
    knightSquares.push_back(currBoard.getPiece(rank + 1, file - 2));
    knightSquares.push_back(currBoard.getPiece(rank - 1, file + 2));
    knightSquares.push_back(currBoard.getPiece(rank + 1, file + 2));

    for (pieceTypes piece: knightSquares) {
        if (piece == enemyKnight) {
            return true;
        }
    }
    return false;
}

bool checkPawnAttackers(Board& currBoard, BoardSquare originSquare, pieceTypes originPiece) {
    std::vector<pieceTypes> possibleAttackers;
    int direction; // rank direction of enemy pawns that can attack you
    if (currBoard.isWhiteTurn) {
        possibleAttackers.push_back(BPawn);
        possibleAttackers.push_back(BPawnJumped);
        direction = -1;
    }
    else {
        possibleAttackers.push_back(WPawn);
        possibleAttackers.push_back(WPawnJumped);
        direction = 1;
    }

    std::vector<pieceTypes> pawnPieces; 
    int rank = originSquare.rank;
    int file = originSquare.file;
    if (file == A) {
        pawnPieces.push_back(currBoard.getPiece(rank + direction, file + 1));
    }
    else if (file == H) {
        pawnPieces.push_back(currBoard.getPiece(rank + direction, file - 1));
    }
    else {
        pawnPieces.push_back(currBoard.getPiece(rank + direction, file - 1));
        pawnPieces.push_back(currBoard.getPiece(rank + direction, file + 1));
    }

    for (pieceTypes piece: pawnPieces) {
        if (std::find(possibleAttackers.begin(), possibleAttackers.end(), piece) != possibleAttackers.end()) {
            return true;
        }
    }
    return false;
}

bool checkKingAttackers(Board& currBoard, BoardSquare originSquare, pieceTypes originPiece) {
    std::vector<pieceTypes> possibleAttackers;
    if (currBoard.isWhiteTurn) {
        possibleAttackers.push_back(BKing);
        possibleAttackers.push_back(BKingUnmoved);
    }
    else {
        possibleAttackers.push_back(WKing);
        possibleAttackers.push_back(WKingUnmoved);
    }

    std::vector<pieceTypes> adjacentPieces; 
    int rank = originSquare.rank;
    int file = originSquare.file;
    adjacentPieces.push_back(currBoard.getPiece(rank + 1, file + 1));
    adjacentPieces.push_back(currBoard.getPiece(rank + 1, file));
    adjacentPieces.push_back(currBoard.getPiece(rank + 1, file - 1));
    adjacentPieces.push_back(currBoard.getPiece(rank, file + 1));
    adjacentPieces.push_back(currBoard.getPiece(rank, file - 1));
    adjacentPieces.push_back(currBoard.getPiece(rank - 1, file - 1));
    adjacentPieces.push_back(currBoard.getPiece(rank - 1, file));
    adjacentPieces.push_back(currBoard.getPiece(rank - 1, file + 1));

    for (pieceTypes piece: adjacentPieces) {
        if (std::find(possibleAttackers.begin(), possibleAttackers.end(), piece) != possibleAttackers.end()) {
            return true;
        }
    }
    return false;
}


bool currKingInAttack(Board currBoard) {
    pieceTypes allyKing = currBoard.isWhiteTurn ? WKing : BKing;
    pieceTypes allyKingUnmoved = currBoard.isWhiteTurn ? WKingUnmoved : BKingUnmoved;
    pieceTypes currKing;

    BoardSquare currKingSquare = BoardSquare();
    for (int rank = 0; rank <= 7; rank++) {
        if (currKingSquare != BoardSquare()) {
            break;
        }
        for (int file = A; file <= H; file++) {
            pieceTypes currPiece = currBoard.getPiece(rank, file);
            if (currPiece == allyKing || currPiece == allyKingUnmoved) {
                currKingSquare = BoardSquare(rank, file);
                currKing = currPiece;
                break;
            }
        }
    }
    
    return checkDiagAttackers(currBoard, currKingSquare, currKing) 
        || checkStraightAttackers(currBoard, currKingSquare, currKing)
        || checkKnightAttackers(currBoard, currKingSquare, currKing)
        || checkPawnAttackers(currBoard, currKingSquare, currKing)
        || checkKingAttackers(currBoard, currKingSquare, currKing);
}


bool notInRange(int var) {return var < 0 || var > 7;}
Board::Board(Board& originalBoard, BoardSquare pos1, BoardSquare pos2, pieceTypes promotionPiece) {
    if (notInRange(pos1.rank) || notInRange(pos1.file) || notInRange(pos2.file) || notInRange(pos2.rank)) {
        this->isIllegalPos = true;
        return;
    }

    this->board = originalBoard.board;
    this->fiftyMoveRule = originalBoard.fiftyMoveRule + 1; // set to 0 in cases with pawn move
    this->isWhiteTurn = originalBoard.isWhiteTurn; // switch turns will happen after the move

    // ally refers to allies of originalBoard, as it is the one still moving this turn
    pieceTypes allyKing = originalBoard.isWhiteTurn ? WKing : BKing;
    pieceTypes allyKingUnmoved = originalBoard.isWhiteTurn ? WKingUnmoved : BKingUnmoved;
    pieceTypes allyRook = originalBoard.isWhiteTurn ? WRook : BRook;
    pieceTypes allyRookUnmoved = originalBoard.isWhiteTurn ? WRookUnmoved : BRookUnmoved;
    pieceTypes allyPawn = originalBoard.isWhiteTurn ? WPawn : BPawn;
    pieceTypes allyPawnJumped = originalBoard.isWhiteTurn ? WPawnJumped : BPawnJumped;
    int pawnJumpDirection = originalBoard.isWhiteTurn ? -2 : 2;
    int promotionRank = originalBoard.isWhiteTurn ? 1 : 6;
    
    pieceTypes originPiece = this->getPiece(pos1);
    pieceTypes targetPiece = this->getPiece(pos2);

    this->setPiece(pos1, EmptyPiece); // origin square should be cleared in all situations
    this->pawnJumped = false;
    this->pawnJumpedSquare = BoardSquare();

    if (originPiece == allyKingUnmoved) {
        // castling
        if (targetPiece == allyRookUnmoved) { 
            // assumes that there are no pieces in between the unmoved king and unmoved rook
            int kingFileDirection = pos2.file > pos1.file ? 1 : -1;
            this->setPiece(pos2, EmptyPiece);
            this->setPiece(pos1.rank, pos1.file + kingFileDirection, allyRook);
            this->setPiece(pos1.rank, pos1.file + kingFileDirection * 2, allyKing);
        }
        else {
            this->setPiece(pos2, allyKing);
        }
    }
    else if (originPiece == allyRookUnmoved) {
        this->setPiece(pos2, allyRook);
    }
    // jumping pawn
    else if (originPiece == allyPawn && pos2.rank == pos1.rank + pawnJumpDirection) { 
            // doesn't check if pawn's original position is rank 2
            this->setPiece(pos2, allyPawnJumped);
            this->pawnJumped = true;
            this->pawnJumpedSquare = pos2;
            this->fiftyMoveRule = 0;
    }
    // promoting pawn
    else if (originPiece == allyPawn && pos1.rank == promotionRank) {
        this->setPiece(pos2, promotionPiece);
        this->fiftyMoveRule = 0;
    }
    // all other pawn moves
    else if (originPiece == allyPawn || originPiece == allyPawnJumped) {
        int fileOffset = pos2.file - pos1.file;
        pieceTypes enemyPawnJumped = originalBoard.isWhiteTurn ? BPawnJumped : WPawnJumped;
        // diagonal movement means capture
        if (fileOffset == 1 || fileOffset == -1) { 
            // en passant
            if (this->getPiece(pos1.rank, pos2.file) == enemyPawnJumped) { 
                originalBoard.pawnJumped = false;
                this->setPiece(pos1.rank, pos2.file, EmptyPiece);
            }
        }
        this->setPiece(pos2, originPiece);
        this->fiftyMoveRule = 0;
    }
    else {
        this->setPiece(pos2, originPiece);
        if (targetPiece != EmptyPiece) {
            this->fiftyMoveRule = 0;
        }
    }

    // if the ally side didn't capture a jumped enemy pawn, disallow future en passants
    if (originalBoard.pawnJumped) {
        pieceTypes enemyPawn = originalBoard.isWhiteTurn ? BPawn : WPawn;
        this->setPiece(originalBoard.pawnJumpedSquare, enemyPawn);
    }

    this->isIllegalPos = currKingInAttack(*this);
    // after finalizing move logic, now switch turns
    this->isWhiteTurn = !originalBoard.isWhiteTurn; 
}

Board::Board(Board& originalBoard, BoardMove move) {
    *this = Board(originalBoard, move.pos1, move.pos2, move.promotionPiece);
}