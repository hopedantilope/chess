// PiecesModel.cpp
#include "PiecesModel.h"

PiecesModel::PiecesModel(QObject* parent)
    : QAbstractListModel(parent) {}

int PiecesModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_pieces.count();
}

QVariant PiecesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    const PieceData& piece = m_pieces[index.row()];
    switch (role) {
        case TypeRole:
            return piece.type;
        case FileIndexRole:
            return piece.fileIndex;
        case RankIndexRole:
            return piece.rankIndex;
        default:
            return QVariant();
    }
}

void PiecesModel::addPiece(const PieceData& piece) {
    beginInsertRows(QModelIndex(), m_pieces.count(), m_pieces.count());
    m_pieces.append(piece);
    endInsertRows();
}

void PiecesModel::clear() {
    beginResetModel();
    m_pieces.clear();
    endResetModel();
}

QHash<int, QByteArray> PiecesModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[FileIndexRole] = "fileIndex";
    roles[RankIndexRole] = "rankIndex";
    return roles;
}
