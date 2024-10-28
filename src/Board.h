#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QStandardItemModel>
#include <QStandardItem>

#include "PiecesModel.h"

class Piece;

class Board : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PiecesModel* piecesModel READ piecesModel NOTIFY piecesModelChanged)


public:
    explicit Board(QObject *parent = nullptr);
    ~Board();

    // Initializes the board with the starting positions of the pieces
    void initializeBoard();

    // Moves a piece from one position to another
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);

    // Retrieves the piece at a specific position
    Piece* getPieceAt(int row, int col) const;

    // Sets a piece at a specific position
    void setPieceAt(int row, int col, Piece* piece);

    // Checks if a position is empty
    bool isEmpty(int row, int col) const;

    Piece *createPieceFromFENChar(char fenChar);
    PiecesModel* piecesModel() const { return m_piecesModel; }


    Q_INVOKABLE void setupBoardFromFEN(const QString& fen);
    Q_INVOKABLE void updatePiecePosition(int index, int fileIndex, int rankIndex);
    Q_INVOKABLE void getLegalMoves();

signals:
    void piecesModelChanged();
    void pieceMoved(int fromRow, int fromCol, int toRow, int toCol);

private:
    PiecesModel* m_piecesModel;
    Piece* boardState[8][8];
};
