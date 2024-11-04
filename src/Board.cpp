// Board.cpp
#include "Board.h"
#include "Pawn.h"
#include "Rook.h"
#include "Queen.h"
#include "Knight.h"
#include "Bishop.h"
#include "King.h"

#include <QDebug>

Board::Board(QObject *parent)
    : QObject(parent), m_piecesModel(new PiecesModel(this)) {
    initializeBoard();
    m_whiteKingX = 4; // e1
    m_whiteKingY = 0;
    m_blackKingX = 4; // e8
    m_blackKingY = 7;
}

Board::~Board() {
    // Clean up allocated Piece objects
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            delete m_boardState[row][col];
            m_boardState[row][col] = nullptr;
        }
    }
}

void Board::initializeBoard() {
    // Initialize all positions to nullptr
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_boardState[row][col] = nullptr;
        }
    }
}

// Set up the board from FEN notation
void Board::setupBoardFromFEN(const QString& fen) {
    initializeBoard(); // Clear the board before setting up

    QStringList parts = fen.split(' ');
    if (parts.isEmpty())
        return;

    QString piecePlacement = parts[0];
    QStringList rows = piecePlacement.split('/');

    m_piecesModel->clear();

    for (int row = 0; row < 8; ++row) {
        int col = 0;
        for (char c : rows[row].toStdString()) {
            if (isdigit(c)) {
                col += c - '0';
            } else {
                Piece* piece = createPieceFromFENChar(c);
                if (piece) {
                    m_boardState[7 - row][col] = piece;
                    PieceData pieceData = { piece->type(), col, 7 - row };
                    m_piecesModel->addPiece(pieceData);
                }
                ++col;
            }
        }
    }
    emit piecesModelChanged();
}

bool Board::updatePiecePosition(int index, int fileIndex, int rankIndex) {
    if (index < 0 || index >= m_piecesModel->rowCount())
        return false;

    QModelIndex modelIndex = m_piecesModel->index(index, 0);
    int oldFileIndex = m_piecesModel->data(modelIndex, PiecesModel::FileIndexRole).toInt();
    int oldRankIndex = m_piecesModel->data(modelIndex, PiecesModel::RankIndexRole).toInt();

    // Validate piece exists and it's their turn
    Piece* movingPiece = m_boardState[oldRankIndex][oldFileIndex];
    if (!movingPiece || movingPiece->getColor() != m_colorsTurn) {
        return false;
    }

    // Validate destination square
    if (fileIndex < 0 || fileIndex >= 8 || rankIndex < 0 || rankIndex >= 8) {
        return false;
    }

    Piece* capturedPiece = m_boardState[rankIndex][fileIndex];
    bool isEnPassantCapture = false;

    // Handle en passant capture
    if (movingPiece->getPieceEnum() == PAWN) {
        int fileDiff = abs(fileIndex - oldFileIndex);
        int rankDiff = rankIndex - oldRankIndex;
        int direction = movingPiece->getColor() == WHITE ? 1 : -1;

        // Check for diagonal move to empty square
        if (capturedPiece == nullptr && fileDiff == 1 && rankDiff == direction) {
            if (rankIndex == m_enPassantTargetRank && fileIndex == m_enPassantTargetFile) {
                // En passant capture is valid
                isEnPassantCapture = true;
                int capturedPawnRank = oldRankIndex;
                int capturedPawnFile = fileIndex;
                Piece* enPassantCapturedPawn = m_boardState[capturedPawnRank][capturedPawnFile];

                // Validate captured pawn
                if (enPassantCapturedPawn && enPassantCapturedPawn->getColor() != movingPiece->getColor() &&
                    enPassantCapturedPawn->getPieceEnum() == PAWN) {

                    // Remove captured pawn from model and board
                    int capturedIndex = findPieceIndex(capturedPawnFile, capturedPawnRank);
                    if (capturedIndex != -1) {
                        m_piecesModel->removePiece(capturedIndex);
                        if (capturedIndex < index) {
                            index--;
                            modelIndex = m_piecesModel->index(index, 0);
                        }
                    }
                    delete enPassantCapturedPawn;
                    m_boardState[capturedPawnRank][capturedPawnFile] = nullptr;
                } else {
                    return false; // Invalid en passant capture
                }
            }
        }
    }

    // Handle normal capture
    if (capturedPiece && !isEnPassantCapture) {
        // Can't capture own piece
        if (capturedPiece->getColor() == movingPiece->getColor()) {
            return false;
        }

        // Remove captured piece from model
        int capturedIndex = findPieceIndex(fileIndex, rankIndex);
        if (capturedIndex != -1) {
            m_piecesModel->removePiece(capturedIndex);
            if (capturedIndex < index) {
                index--;
                modelIndex = m_piecesModel->index(index, 0);
            }
        }
        delete capturedPiece;
    }

    // Update the board state
    m_boardState[oldRankIndex][oldFileIndex] = nullptr;
    m_boardState[rankIndex][fileIndex] = movingPiece;

    // Update the model
    bool success = m_piecesModel->setData(modelIndex, fileIndex, PiecesModel::FileIndexRole) &&
                   m_piecesModel->setData(modelIndex, rankIndex, PiecesModel::RankIndexRole);

    if (success) {
        emit pieceMoved(oldRankIndex, oldFileIndex, rankIndex, fileIndex);
    } else {
        // Revert board state if model update fails
        m_boardState[oldRankIndex][oldFileIndex] = movingPiece;
        m_boardState[rankIndex][fileIndex] = capturedPiece;
        return false;
    }

    // Update whose turn it is
    m_colorsTurn = (m_colorsTurn == WHITE) ? BLACK : WHITE;

    // Update king's position if moved
    if (movingPiece->getPieceEnum() == KING) {
        if (movingPiece->getColor() == WHITE) {
            m_whiteKingMoved = true;
            m_whiteKingX = fileIndex;
            m_whiteKingY = rankIndex;
        } else {
            m_blackKingMoved = true;
            m_blackKingX = fileIndex;
            m_blackKingY = rankIndex;
        }
    }

    if (movingPiece->getPieceEnum() == ROOK) {
        if (movingPiece->getColor() == WHITE) {
            if (oldFileIndex == 0 && oldRankIndex == 0) {
                m_whiteQueensideRookMoved = true;
            } else if (oldFileIndex == 7 && oldRankIndex == 0) {
                m_whiteKingsideRookMoved = true;
            }
        } else {
            if (oldFileIndex == 0 && oldRankIndex == 7) {
                m_blackQueensideRookMoved = true;
            } else if (oldFileIndex == 7 && oldRankIndex == 7) {
                m_blackKingsideRookMoved = true;
            }
        }
    }

    // Reset en passant target
    m_enPassantTargetRank = -1;
    m_enPassantTargetFile = -1;

    // Set en passant target if pawn moved two squares
    if (movingPiece->getPieceEnum() == PAWN) {
        if (abs(rankIndex - oldRankIndex) == 2) {
            m_enPassantTargetRank = (oldRankIndex + rankIndex) / 2;
            m_enPassantTargetFile = fileIndex;
        }
    }

    return success;
}


int Board::findPieceIndex(int fileIndex, int rankIndex) {
    for (int i = 0; i < m_piecesModel->rowCount(); ++i) {
        QModelIndex idx = m_piecesModel->index(i, 0);
        if (m_piecesModel->data(idx, PiecesModel::FileIndexRole).toInt() == fileIndex &&
            m_piecesModel->data(idx, PiecesModel::RankIndexRole).toInt() == rankIndex) {
            return i;
        }
    }
    return -1;
}

Piece* Board::createPieceFromFENChar(char fenChar) {
    bool isWhite = isupper(fenChar);
    fenChar = tolower(fenChar);

    switch (fenChar) {
        case 'p': return new Pawn(isWhite ? WHITE : BLACK);
        case 'r': return new Rook(isWhite ? WHITE : BLACK);
        case 'n': return new Knight(isWhite ? WHITE : BLACK);
        case 'b': return new Bishop(isWhite ? WHITE : BLACK);
        case 'q': return new Queen(isWhite ? WHITE : BLACK);
        case 'k': return new King(isWhite ? WHITE : BLACK);
        default: return nullptr; // Invalid character
    }
}

QVariantList Board::getPseudoLegalMoves(int rankIndex, int fileIndex) {
    if (rankIndex < 0 || rankIndex >= 8 || fileIndex < 0 || fileIndex >= 8)
        return {};

    Piece* piece = m_boardState[rankIndex][fileIndex];
    if (!piece)
        return {};

    Color color = piece->getColor();
    PieceType pieceType = piece->getPieceEnum();

    switch (pieceType) {
        case PAWN:
            return computePawnPseudoLegalMoves(rankIndex, fileIndex, color);
        case ROOK:
            return computeRookPseudoLegalMoves(rankIndex, fileIndex, color);
        case KNIGHT:
            return computeKnightPseudoLegalMoves(rankIndex, fileIndex, color);
        case BISHOP:
            return computeBishopPseudoLegalMoves(rankIndex, fileIndex, color);
        case QUEEN:
            return computeQueenPseudoLegalMoves(rankIndex, fileIndex, color);
        case KING:
            return computeKingPseudoLegalMoves(rankIndex, fileIndex, color);
        default:
            return {};
    }
}

QVariantList Board::computePawnPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int direction = (color == WHITE) ? 1 : -1;
    int startRank = (color == WHITE) ? 1 : 6;
    int enPassantRank = (color == WHITE) ? 4 : 3; // Rank where en passant is possible
    int nextRank = rankIndex + direction;

    // Move forward one square
    if (nextRank >= 0 && nextRank < 8 && !m_boardState[nextRank][fileIndex]) {
        QVariantMap pointMap;
        pointMap["x"] = fileIndex;
        pointMap["y"] = nextRank;
        moves.append(pointMap);

        // Move forward two squares from starting position
        if (rankIndex == startRank) {
            int nextRank2 = rankIndex + 2 * direction;
            if (!m_boardState[nextRank2][fileIndex]) {
                QVariantMap pointMap2;
                pointMap2["x"] = fileIndex;
                pointMap2["y"] = nextRank2;
                moves.append(pointMap2);
            }
        }
    }

    // Regular captures
    for (int dFile = -1; dFile <= 1; dFile += 2) {
        int newFileIndex = fileIndex + dFile;
        if (newFileIndex >= 0 && newFileIndex < 8) {
            int captureRank = nextRank;
            if (captureRank >= 0 && captureRank < 8) {
                Piece* targetPiece = m_boardState[captureRank][newFileIndex];
                if (targetPiece && targetPiece->getColor() != color) {
                    QVariantMap pointMap;
                    pointMap["x"] = newFileIndex;
                    pointMap["y"] = captureRank;
                    moves.append(pointMap);
                }
            }
        }
    }

    // En passant captures
    if (rankIndex == enPassantRank) {
        for (int dFile = -1; dFile <= 1; dFile += 2) {
            int newFileIndex = fileIndex + dFile;
            if (newFileIndex >= 0 && newFileIndex < 8) {
                if (m_enPassantTargetRank == nextRank && m_enPassantTargetFile == newFileIndex) {
                    // En passant capture is possible
                    QVariantMap pointMap;
                    pointMap["x"] = newFileIndex;
                    pointMap["y"] = nextRank;
                    moves.append(pointMap);
                }
            }
        }
    }

    // TODO: Promotion handling

    return moves;
}


QVariantList Board::computeRookPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int directions[4][2] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };
    for (auto& dir : directions) {
        int x = fileIndex;
        int y = rankIndex;
        while (true) {
            x += dir[0];
            y += dir[1];

            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;

            Piece* targetPiece = m_boardState[y][x];
            QVariantMap pointMap;
            pointMap["x"] = x;
            pointMap["y"] = y;

            if (!targetPiece) {
                moves.append(pointMap);
            } else {
                if (targetPiece->getColor() != color)
                    moves.append(pointMap);
                break;
            }
        }
    }
    return moves;
}

QVariantList Board::computeKnightPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int movesOffsets[8][2] = {
        { -2, -1 }, { -2, 1 }, { -1, -2 }, { -1, 2 },
        { 1, -2 },  { 1, 2 },  { 2, -1 },  { 2, 1 }
    };
    for (auto& offset : movesOffsets) {
        int x = fileIndex + offset[0];
        int y = rankIndex + offset[1];

        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece* targetPiece = m_boardState[y][x];
            if (!targetPiece || targetPiece->getColor() != color) {
                QVariantMap pointMap;
                pointMap["x"] = x;
                pointMap["y"] = y;
                moves.append(pointMap);
            }
        }
    }
    return moves;
}

QVariantList Board::computeBishopPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int directions[4][2] = { { -1, 1 }, { 1, 1 }, { -1, -1 }, { 1, -1 } };
    for (auto& dir : directions) {
        int x = fileIndex;
        int y = rankIndex;
        while (true) {
            x += dir[0];
            y += dir[1];

            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;

            Piece* targetPiece = m_boardState[y][x];
            QVariantMap pointMap;
            pointMap["x"] = x;
            pointMap["y"] = y;

            if (!targetPiece) {
                moves.append(pointMap);
            } else {
                if (targetPiece->getColor() != color)
                    moves.append(pointMap);
                break;
            }
        }
    }
    return moves;
}

QVariantList Board::computeQueenPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int directions[8][2] = {
        { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 },
        { -1, 1 }, { 1, 1 }, { -1, -1 }, { 1, -1 }
    };
    for (auto& dir : directions) {
        int x = fileIndex;
        int y = rankIndex;
        while (true) {
            x += dir[0];
            y += dir[1];

            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;

            Piece* targetPiece = m_boardState[y][x];
            QVariantMap pointMap;
            pointMap["x"] = x;
            pointMap["y"] = y;

            if (!targetPiece) {
                moves.append(pointMap);
            } else {
                if (targetPiece->getColor() != color)
                    moves.append(pointMap);
                break;
            }
        }
    }
    return moves;
}

QVariantList Board::computeKingPseudoLegalMoves(int rankIndex, int fileIndex, Color color) {
    QVariantList moves;
    int directions[8][2] = {
        { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 },
        { -1, 1 }, { 1, 1 }, { -1, -1 }, { 1, -1 }
    };

    Piece* movingPiece = m_boardState[rankIndex][fileIndex];
    if (!movingPiece || movingPiece->getPieceEnum() != KING || movingPiece->getColor() != color) {
        return moves; // No king to move
    }

    for (auto& dir : directions) {
        int targetX = fileIndex + dir[0];
        int targetY = rankIndex + dir[1];

        if (targetX >= 0 && targetX < 8 && targetY >= 0 && targetY < 8) {
            Piece* targetPiece = m_boardState[targetY][targetX];
            if (!targetPiece || targetPiece->getColor() != color) {

                // Check if king is not in check after the move
                bool moveIsLegal = isMoveLegal(rankIndex, fileIndex, targetY, targetX, color);

                // If move is legal, the king is not in check
                if (moveIsLegal) {
                    QVariantMap moveMap;
                    moveMap["x"] = targetX;
                    moveMap["y"] = targetY;
                    moves.append(moveMap);
                }
            }
        }
    }

    // Castling Checks
    // Check if the king has already moved
    if ((color == WHITE && m_whiteKingMoved) || (color == BLACK && m_blackKingMoved)) {
        return moves; // King has moved, cannot castle
    }

    if (!isKingInCheck(color)) {
        // --- Kingside Castling ---
        bool canCastleKingside = true;
        Piece* kingsideRook = nullptr;
        if (color == WHITE) {
            kingsideRook = m_boardState[0][7];
        } else {
            kingsideRook = m_boardState[7][7];
        }

        // Check if the kingside rook exists, is a rook of the correct color, and hasn't moved
        if (!kingsideRook || kingsideRook->getPieceEnum() != ROOK || kingsideRook->getColor() != color ||
            (color == WHITE ? m_whiteKingsideRookMoved : m_blackKingsideRookMoved)) {
            canCastleKingside = false;
        } else {
            // Define squares between king and kingside rook
            int fFile = 5, gFile = 6;
            int rank = (color == WHITE) ? 0 : 7;

            // Check if squares f and g are empty
            if (m_boardState[rank][fFile] != nullptr || m_boardState[rank][gFile] != nullptr) {
                canCastleKingside = false;
            }

            // Check if squares f and g are under attack
            if (canCastleKingside) {
                bool fUnderAttack = !isMoveLegal(rank, 4, rank, fFile, color);
                bool gUnderAttack = !isMoveLegal(rank, 4, rank, gFile, color);
                if (fUnderAttack || gUnderAttack) {
                    canCastleKingside = false;
                }
            }
        }

        if (canCastleKingside) {
            QVariantMap castlingMove;
            castlingMove["x"] = 6;
            castlingMove["y"] = (color == WHITE) ? 0 : 7;
            moves.append(castlingMove);
        }

        // --- Queenside Castling ---
        bool canCastleQueenside = true;
        Piece* queensideRook = nullptr;
        if (color == WHITE) {
            queensideRook = m_boardState[0][0];
        } else {
            queensideRook = m_boardState[7][0];
        }

        // Check if the queenside rook exists, is a rook of the correct color, and hasn't moved
        if (!queensideRook || queensideRook->getPieceEnum() != ROOK || queensideRook->getColor() != color ||
            (color == WHITE ? m_whiteQueensideRookMoved : m_blackQueensideRookMoved)) {
            canCastleQueenside = false;
        } else {
            // Define squares between king and queenside rook
            int dFile = 3, cFile = 2, bFile = 1;
            int rank = (color == WHITE) ? 0 : 7;

            // Check if squares d, c, and b are empty
            if (m_boardState[rank][dFile] != nullptr || m_boardState[rank][cFile] != nullptr || m_boardState[rank][bFile] != nullptr) {
                canCastleQueenside = false;
            }

            // Check if squares d and c are under attack
            if (canCastleQueenside) {
                bool dUnderAttack = !isMoveLegal(rank, 4, rank, dFile, color);
                bool cUnderAttack = !isMoveLegal(rank, 4, rank, cFile, color);
                if (dUnderAttack || cUnderAttack) {
                    canCastleQueenside = false;
                }
            }
        }

        if (canCastleQueenside) {
            QVariantMap castlingMove;
            castlingMove["x"] = 2;
            castlingMove["y"] = (color == WHITE) ? 0 : 7;
            moves.append(castlingMove);
        }
    }

    return moves;
}

// Optimized isKingInCheck function using stored king positions
bool Board::isKingInCheck(Color color) {
    int kingX, kingY;
    if (color == WHITE) {
        kingX = m_whiteKingX;
        kingY = m_whiteKingY;
    } else {
        kingX = m_blackKingX;
        kingY = m_blackKingY;
    }

    Color opponentColor = (color == WHITE) ? BLACK : WHITE;

    // Check for attacks from pawns
    int pawnDirection = (color == WHITE) ? 1 : -1;
    for (int dx = -1; dx <= 1; dx += 2) {
        int x = kingX + dx;
        int y = kingY + pawnDirection;
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece* piece = m_boardState[y][x];
            if (piece && piece->getColor() == opponentColor && piece->getPieceEnum() == PAWN)
                return true;
        }
    }

    // Check for attacks from knights
    int knightMoves[8][2] = {
        { -2, -1 }, { -2, 1 }, { -1, -2 }, { -1, 2 },
        { 1, -2 },  { 1, 2 },  { 2, -1 },  { 2, 1 }
    };
    for (auto& move : knightMoves) {
        int x = kingX + move[0];
        int y = kingY + move[1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece* piece = m_boardState[y][x];
            if (piece && piece->getColor() == opponentColor && piece->getPieceEnum() == KNIGHT)
                return true;
        }
    }

    // Check for attacks from bishops and queens (diagonals)
    int bishopDirections[4][2] = { { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, 1 } };
    for (auto& dir : bishopDirections) {
        int x = kingX;
        int y = kingY;
        while (true) {
            x += dir[0];
            y += dir[1];
            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;
            Piece* piece = m_boardState[y][x];
            if (piece) {
                if (piece->getColor() == opponentColor) {
                    PieceType pt = piece->getPieceEnum();
                    if (pt == BISHOP || pt == QUEEN)
                        return true;
                }
                break; // Blocked
            }
        }
    }

    // Check for attacks from rooks and queens (straight lines)
    int rookDirections[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
    for (auto& dir : rookDirections) {
        int x = kingX;
        int y = kingY;
        while (true) {
            x += dir[0];
            y += dir[1];
            if (x < 0 || x >= 8 || y < 0 || y >= 8)
                break;
            Piece* piece = m_boardState[y][x];
            if (piece) {
                if (piece->getColor() == opponentColor) {
                    PieceType pt = piece->getPieceEnum();
                    if (pt == ROOK || pt == QUEEN)
                        return true;
                }
                break; // Blocked
            }
        }
    }

    // Check for attacks from the opponent king (adjacent squares)
    int kingMoves[8][2] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 },
        { 0, -1 },            { 0, 1 },
        { 1, -1 },  { 1, 0 }, { 1, 1 }
    };
    for (auto& move : kingMoves) {
        int x = kingX + move[0];
        int y = kingY + move[1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece* piece = m_boardState[y][x];
            if (piece && piece->getColor() == opponentColor && piece->getPieceEnum() == KING)
                return true;
        }
    }

    return false;
}

bool Board::isMoveLegal(int fromRank, int fromFile, int toRank, int toFile, Color color) {
    // Save the original pieces
    Piece* movingPiece = m_boardState[fromRank][fromFile];
    Piece* capturedPiece = m_boardState[toRank][toFile];

    // Make the move
    m_boardState[fromRank][fromFile] = nullptr;
    m_boardState[toRank][toFile] = movingPiece;

    // Update king position if moving a king
    int originalKingX = -1, originalKingY = -1;
    PieceType pieceType = movingPiece->getPieceEnum();
    if (pieceType == KING) {
        originalKingX = (color == WHITE) ? m_whiteKingX : m_blackKingX;
        originalKingY = (color == WHITE) ? m_whiteKingY : m_blackKingY;

        if (color == WHITE) {
            m_whiteKingX = toFile;
            m_whiteKingY = toRank;
        } else {
            m_blackKingX = toFile;
            m_blackKingY = toRank;
        }
    }

    // Check if king is in check
    bool kingInCheck = isKingInCheck(color);

    // Undo the move
    m_boardState[fromRank][fromFile] = movingPiece;
    m_boardState[toRank][toFile] = capturedPiece;

    // Restore king position if necessary
    if (pieceType == KING) {
        if (color == WHITE) {
            m_whiteKingX = originalKingX;
            m_whiteKingY = originalKingY;
        } else {
            m_blackKingX = originalKingX;
            m_blackKingY = originalKingY;
        }
    }

    return !kingInCheck;
}

// Updated getLegalMoves with helper functions
QVariantList Board::getLegalMoves(int rankIndex, int fileIndex) {
    QVariantList moves;

    if (rankIndex < 0 || rankIndex >= 8 || fileIndex < 0 || fileIndex >= 8)
        return moves;

    Piece* piece = m_boardState[rankIndex][fileIndex];
    if (!piece)
        return moves;

    if (piece->getColor() != m_colorsTurn)
        return moves;

    Color color = piece->getColor();

    QVariantList pseudoMoves = getPseudoLegalMoves(rankIndex, fileIndex);

    for (const QVariant& move : pseudoMoves) {
        QVariantMap moveMap = move.toMap();
        int toX = moveMap["x"].toInt();
        int toY = moveMap["y"].toInt();

        if (isMoveLegal(rankIndex, fileIndex, toY, toX, color)) {
            moves.append(moveMap);
        }
    }

    return moves;
}
