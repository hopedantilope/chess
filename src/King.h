#pragma once
#include "Piece.h"

class King : public Piece
{
    Q_OBJECT
public:
    explicit King(Color color, QObject *parent = nullptr);
    ~King();

    QString type() const override;
    QList<Move>  validMoveList() const override;
    PieceType getPieceEnum() const override{return KING;}

};

