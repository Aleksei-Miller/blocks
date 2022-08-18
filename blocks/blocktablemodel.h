#ifndef BLOCKTABLEMODEL_H
#define BLOCKTABLEMODEL_H

#include <QAbstractTableModel>

#include <QFile>
#include <QSaveFile>

#include <QDir>

#include "driverblock.h"
#include "driver2block.h"

class BlockTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BlockTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //
    bool importMission(const QModelIndex &index, const QString &path);
    bool exportMission(const QModelIndex &index, const QString &path);

    QString path() const;
    bool setPath(const QString &path);

    bool sync();

    Block blockType() const;
    const QStringList &gameModeList();

signals:
    void error(const QString &e);

private:
    QString m_path;

    AbstractBlock *m_currentBlock;

    DriverBlock *m_driverBlock;
    Driver2Block *m_driver2Block;

    Block m_blockType;

    QStringList m_gameModeList;

    bool readFileData(const QString &path, QByteArray &buffer, bool silentMode = false);
    bool writeFileData(const QString &path, const QByteArray &buffer);
};

#endif // BLOCKTABLEMODEL_H
