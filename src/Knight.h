#pragma once
#include "Piece.h"

class Knight : public Piece
{
    Q_OBJECT
public:
    explicit Knight(Color color, QObject *parent = nullptr);
    ~Knight();

    QString type() const override;
    QList<Move>  validMoveList() const override;
};

