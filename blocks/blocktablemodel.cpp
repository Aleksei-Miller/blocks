#include "blocktablemodel.h"

BlockTableModel::BlockTableModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    m_gameModeList << tr("None") << tr("Undercover")
                   << tr("Take a ride") << tr("Quick chase")
                   << tr("Quick Gateway") << tr("Gate racing")
                   << tr("Trailblazer") << tr("Checkpoint")
                   << tr("Survival") << tr("Secret race")
                   << tr("Replay") << tr("MP take a ride")
                   << tr("MP checkpoint") << tr("MP capture the flag")
                   << tr("MP cops'n'robbers") << tr("MP secret race")
                   << tr("Dirt track") << tr("Training");

    m_blockType = Block::Unknown;

    m_currentBlock = nullptr;
    m_driverBlock = new DriverBlock(this);
    m_driver2Block = new Driver2Block(this);
}

QByteArray BlockTableModel::blockData() const
{
    return m_currentBlock->data();
}

bool BlockTableModel::setBlockData(const QByteArray &data, const QString &missionNames)
{
    beginResetModel();

    m_driverBlock->clearData();
    m_driver2Block->clearData();

    if (Driver2Block::hasSignature(data)) {
        m_blockType = Block::Driver2;
        m_currentBlock = m_driver2Block;

        m_driver2Block->setMissionNames(missionNames);
    }
    else {
        m_blockType = Block::Driver;
        m_currentBlock = m_driverBlock;
    }

    if (!m_currentBlock->setData(data)) {
        m_currentBlock = nullptr;

        endResetModel();
        return false;
    }

    endResetModel();
    return true;
}

Block BlockTableModel::blockType() const
{
    return m_blockType;
}

const QStringList &BlockTableModel::gameModeList()
{
    return m_gameModeList;
}

int BlockTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_currentBlock == nullptr)
        return 0;

    return m_currentBlock->count();
}

int BlockTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ?  0 : 7;
}

QVariant BlockTableModel::data(const QModelIndex &index, int role) const
{ 
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        int row = index.row();
        MissionEntry entry = m_currentBlock->at(row);

        switch (index.column()) {
        case 0: return entry.id;

        case 1: return (entry.name != "" ) ? entry.name : m_gameModeList.at(entry.mode);

        case 2: return entry.city;

        case 3: return entry.time;

        case 4: return entry.weather;

        case 5: return entry.data.size();

        case 6: return entry.mode;
        }
    }
    else if (role == Qt::TextAlignmentRole && index.column() > 1)
        return Qt::AlignCenter;

    return QVariant();
}

const QByteArray *BlockTableModel::missionData(const QModelIndex &index) const
{
    return index.isValid() ? &m_currentBlock->at(index.row()).data : nullptr;
}

bool BlockTableModel::setMissionData(const QModelIndex &index, const QByteArray &data)
{
    if (!index.isValid())
        return false;

    if (!m_currentBlock->setMissionData(index.row(), data))
        return false;

    return true;
}

QVariant BlockTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return tr("ID");
        case 1: return tr("Name");
        case 2: return tr("City");
        case 3: return tr("Time");
        case 4: return tr("Weather");
        case 5: return tr("Size");
        case 6: return tr("Game mode");
        }
    }

    return QVariant();
}
