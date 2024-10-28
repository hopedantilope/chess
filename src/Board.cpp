#include "Board.h"
#include "Piece.h"
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
}

Board::~Board()
{
    // Clean up all allocated Piece objects
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            delete boardState[row][col];
            boardState[row][col] = nullptr;
        }
    }
}

void Board::initializeBoard()
{
    // Initialize all positions to nullptr
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            boardState[row][col] = nullptr;
        }
    }
}

// Method to set up the board based on FEN notation
void Board::setupBoardFromFEN(const QString& fen)
{
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
                if (piece != nullptr) {
                    boardState[7 - row][col] = piece;
                    PieceData pieceData;
                    pieceData.type = piece->type();
                    pieceData.fileIndex = col;
                    pieceData.rankIndex = 7 - row;
                    m_piecesModel->addPiece(pieceData);
                }
                ++col;
            }
        }
    }
    emit piecesModelChanged();
}

void Board::updatePiecePosition(int index, int fileIndex, int rankIndex) {
    if (index < 0 || index >= m_piecesModel->rowCount())
        return;

    QModelIndex modelIndex = m_piecesModel->index(index, 0);
    m_piecesModel->setData(modelIndex, fileIndex, PiecesModel::FileIndexRole);
    m_piecesModel->setData(modelIndex, rankIndex, PiecesModel::RankIndexRole);
}

Piece* Board::createPieceFromFENChar(char fenChar)
{
    bool isWhite = isupper(fenChar);
    fenChar = tolower(fenChar);

    switch (fenChar)
    {
        case 'p': return new Pawn(isWhite ? WHITE : BLACK);
        case 'r': return new Rook(isWhite ? WHITE : BLACK);
        case 'n': return new Knight(isWhite ? WHITE : BLACK);
        case 'b': return new Bishop(isWhite ? WHITE : BLACK);
        case 'q': return new Queen(isWhite ? WHITE : BLACK);
        case 'k': return new King(isWhite ? WHITE : BLACK);
        default: return nullptr; // Invalid character
    }
}

void Board::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
    Piece* piece = getPieceAt(fromRow, fromCol);
    if (piece)
    {
        // Optional: Validate the move
        Piece* capturedPiece = getPieceAt(toRow, toCol);
        if (capturedPiece)
        {
            delete capturedPiece;
        }
        setPieceAt(toRow, toCol, piece);
        setPieceAt(fromRow, fromCol, nullptr);

        emit pieceMoved(fromRow, fromCol, toRow, toCol);
    }
}

Piece* Board::getPieceAt(int row, int col) const
{
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
    {
        return boardState[row][col];
    }
    return nullptr;
}

void Board::setPieceAt(int row, int col, Piece* piece)
{
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
    {
        boardState[row][col] = piece;
    }
}

bool Board::isEmpty(int row, int col) const
{
    return getPieceAt(row, col) == nullptr;
}
