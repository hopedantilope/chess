#pragma once

#include <QObject>
#include <QList>

enum Color {
    WHITE,
    BLACK
};

struct Move {
    int x;
    int y;
    Move(int x, int y) : x(x), y(y) {}
};

enum PieceType
{
    KING,
    QUEEN,
    BISHOP,
    KNIGHT,
    ROOK,
    PAWN
};

class Piece : public QObject
{
    Q_OBJECT
public:
    explicit Piece(Color color, QObject *parent = nullptr);
    virtual ~Piece();  // Virtual destructor to ensure proper cleanup when derived classes are deleted

    // Common piece methods can be added here
    virtual QString type() const = 0;  // Pure virtual function to be implemented by each specific piece type
    virtual PieceType getPieceEnum() const = 0;
    QString getColorString() const;
    virtual QList<Move>  validMoveList() const = 0;
    Color getColor(){return m_color;}
    Color m_color;
};
