#include "King.h"

King::King(Color color, QObject *parent) : Piece(color, parent)
{
}

King::~King()
{
}

QString King::type() const
{
    QString pieceType = getColorString() + "king";
    return pieceType;
}

QList<Move> King::validMoveList() const
{
    QList<Move> validMoves;
    return validMoves;
}
