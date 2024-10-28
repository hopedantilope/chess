#include "Piece.h"

Piece::Piece(Color color, QObject *parent) : QObject(parent), m_color(color)
{
    // General piece constructor (can be left empty for now)
}

Piece::~Piece()
{
    // Virtual destructor ensures that the correct destructor is called for derived classes
}

QString Piece::getColorString() const {
    if (m_color == WHITE) {
        return "white-";
    } else {
        return "black-";
    }
}