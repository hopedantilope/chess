#include "PiecesModel.h"
#include <QDebug>
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

bool PiecesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= m_pieces.count()) {
        return false;
    }

    PieceData& piece = m_pieces[index.row()];
    bool changed = false;

    switch (role) {
        case TypeRole:
            if (piece.type != value.toString()) {
                piece.type = value.toString();
                changed = true;
            }
            break;
        case FileIndexRole:
            if (piece.fileIndex != value.toInt()) {
                piece.fileIndex = value.toInt();
                changed = true;
            }
            break;
        case RankIndexRole:
            if (piece.rankIndex != value.toInt()) {
                piece.rankIndex = value.toInt();
                changed = true;
            }
            break;
        default:
            return false;
    }

    if (changed) {
        emit dataChanged(index, index, {role});
    }

    return true;
}


void PiecesModel::addPiece(const PieceData& piece) {
    beginInsertRows(QModelIndex(), m_pieces.count(), m_pieces.count());
    m_pieces.append(piece);
    endInsertRows();
}

void PiecesModel::removePiece(int index) {
    if (index >= 0 && index < m_pieces.count()) {
        beginRemoveRows(QModelIndex(), index, index);
        m_pieces.removeAt(index);
        endRemoveRows();
    }
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