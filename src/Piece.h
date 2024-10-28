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

class Piece : public QObject
{
    Q_OBJECT
public:
    explicit Piece(Color color, QObject *parent = nullptr);
    virtual ~Piece();  // Virtual destructor to ensure proper cleanup when derived classes are deleted

    // Common piece methods can be added here
    virtual QString type() const = 0;  // Pure virtual function to be implemented by each specific piece type
    QString getColorString() const;
    virtual QList<Move>  validMoveList() const = 0;
    Color m_color;
};
