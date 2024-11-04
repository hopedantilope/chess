#pragma once
#include "Piece.h"

class Queen : public Piece
{
    Q_OBJECT
public:
    explicit Queen(Color color, QObject *parent = nullptr);
    ~Queen();

    QString type() const override;
    QList<Move>  validMoveList() const override;
    PieceType getPieceEnum() const override{return QUEEN;}

};
