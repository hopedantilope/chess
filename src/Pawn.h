#pragma once
#include "Piece.h"

class Pawn : public Piece
{
    Q_OBJECT
public:
    explicit Pawn(Color color, QObject *parent = nullptr);
    ~Pawn();

    // Implement the type method from Piece
    QString type() const override;

    // Implement the movement rules for the Pawn
    QList<Move>  validMoveList() const override;
    PieceType getPieceEnum() const override{return PAWN;}

};
