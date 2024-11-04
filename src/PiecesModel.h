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
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    void addPiece(const PieceData& piece);
    void removePiece(int index);
    void clear();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<PieceData> m_pieces;
};