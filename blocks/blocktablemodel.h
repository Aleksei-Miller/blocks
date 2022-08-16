/*!
 * \file
 * \brief MModel fro manipilate blk files.
 */

#ifndef BLOCKTABLEMODEL_H
#define BLOCKTABLEMODEL_H

#include <QAbstractTableModel>

#include <QDebug>

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
    bool setMissionData(const QModelIndex &index, const QByteArray &data);
    const QByteArray *missionData(const QModelIndex &index) const;

    QByteArray blockData() const;
    bool setBlockData(const QByteArray &data, const QString &missionNames = "");

    Block blockType() const;
    const QStringList &gameModeList();

private:
    AbstractBlock *m_currentBlock;

    DriverBlock *m_driverBlock;
    Driver2Block *m_driver2Block;

    Block m_blockType;

    QStringList m_gameModeList;
};

#endif // BLOCKTABLEMODEL_H
