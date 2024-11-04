#include "Piece.h"

Piece::Piece(Color color, QObject *parent) : QObject(parent), m_color(color)
{
}

Piece::~Piece()
{
}

QString Piece::getColorString() const {
    if (m_color == WHITE) {
        return "white-";
    } else {
        return "black-";
    }
}