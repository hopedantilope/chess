#include "Queen.h"

Queen::Queen(Color color, QObject *parent) : Piece(color, parent)
{
}

Queen::~Queen()
{
}

QString Queen::type() const
{
    QString pieceType = getColorString() + "queen";
    return pieceType;
}

QList<Move> Queen::validMoveList() const
{
    QList<Move> validMoves;
    return validMoves;
}
