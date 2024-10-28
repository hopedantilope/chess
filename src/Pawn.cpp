#include "Pawn.h"

Pawn::Pawn(Color color, QObject *parent) : Piece(color, parent)
{
    // Pawn-specific initialization, if needed
}

Pawn::~Pawn()
{
    // Destructor for the Pawn class
}

QString Pawn::type() const {
    QString pieceType = getColorString() + "pawn";
    return pieceType;
}


QList<Move> Pawn::validMoveList() const
{
    QList<Move> validMoves;
    return validMoves;  // Invalid move
}
