#include "Knight.h"

Knight::Knight(Color color, QObject *parent) : Piece(color, parent)
{
}

Knight::~Knight()
{
}

QString Knight::type() const
{
    QString pieceType = getColorString() + "knight";
    return pieceType;
}

QList<Move> Knight::validMoveList() const
{
    QList<Move> validMoves;
    return validMoves;
}
