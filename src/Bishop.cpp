#include "Bishop.h"

Bishop::Bishop(Color color, QObject *parent) : Piece(color, parent)
{
}

Bishop::~Bishop()
{
}

QString Bishop::type() const
{
    QString pieceType = getColorString() + "bishop";
    return pieceType;
}

QList<Move> Bishop::validMoveList() const
{
    QList<Move> validMoves;
    return validMoves;
}
