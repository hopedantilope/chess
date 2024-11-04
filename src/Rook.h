#pragma once
#include "Piece.h"

class Rook : public Piece
{
    Q_OBJECT
public:
    explicit Rook(Color color, QObject *parent = nullptr);
    ~Rook();

    QString type() const override;
    QList<Move>  validMoveList() const override;
    PieceType getPieceEnum() const override{return ROOK;}
};
