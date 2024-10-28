#include "Rook.h"

Rook::Rook(Color color, QObject *parent) : Piece(color, parent)
{
}

Rook::~Rook()
{
}

QString Rook::type() const
{
    QString pieceType = getColorString() + "rook";
    return pieceType;
}

QList<Move> Rook::validMoveList() const
{
    QList<Move> validMoves;

    return validMoves;
}
