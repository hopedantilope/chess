// Board.h
#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QStandardItemModel>
#include "Piece.h"
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

    void initializeBoard();
    Piece *createPieceFromFENChar(char fenChar);
    PiecesModel* piecesModel() const { return m_piecesModel; }
    int findPieceIndex(int fileIndex, int rankIndex);
    QVariantList getPseudoLegalMoves(int rankIndex, int fileIndex);
    bool isKingInCheck(Color color);

    Q_INVOKABLE void setupBoardFromFEN(const QString& fen);
    Q_INVOKABLE bool updatePiecePosition(int index, int fileIndex, int rankIndex);
    Q_INVOKABLE QVariantList getLegalMoves(int rankIndex, int fileIndex);
    
signals:
    void piecesModelChanged();
    void pieceMoved(int fromRow, int fromCol, int toRow, int toCol);

private:
    int m_enPassantTargetRank = -1;
    int m_enPassantTargetFile = -1;

    bool m_whiteKingMoved = false;
    bool m_whiteKingsideRookMoved = false;
    bool m_whiteQueensideRookMoved = false;
    bool m_blackKingMoved = false;
    bool m_blackKingsideRookMoved = false;
    bool m_blackQueensideRookMoved = false;

    int m_whiteKingX, m_whiteKingY;
    int m_blackKingX, m_blackKingY;
    PiecesModel* m_piecesModel;
    Piece* m_boardState[8][8];
    Color m_colorsTurn = WHITE;
    bool m_promote = false;
    // Helper functions
    QVariantList computePawnPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    QVariantList computeRookPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    QVariantList computeKnightPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    QVariantList computeBishopPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    QVariantList computeQueenPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    QVariantList computeKingPseudoLegalMoves(int rankIndex, int fileIndex, Color color);
    bool isMoveLegal(int fromRank, int fromFile, int toRank, int toFile, Color color);

    bool isSquareUnderAttack(int rank, int file, Color attackerColor);
    bool canCastleKingside(Color color);
    bool canCastleQueenside(Color color);
};
