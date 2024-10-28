// PiecesModel.h
#pragma once
#include <QAbstractListModel>
#include <QObject>

struct PieceData {
    QString type;
    int fileIndex;
    int rankIndex;
};

class PiecesModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        FileIndexRole,
        RankIndexRole
    };

    PiecesModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addPiece(const PieceData& piece);
    void clear();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<PieceData> m_pieces;
};
