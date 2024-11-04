#pragma once
#include "Piece.h"

class Bishop : public Piece
{
    Q_OBJECT
public:
    explicit Bishop(Color color, QObject *parent = nullptr);
    ~Bishop();

    QString type() const override;
    QList<Move>  validMoveList() const override;
    PieceType getPieceEnum() const override{return BISHOP;}

};
