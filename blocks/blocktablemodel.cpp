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

bool BlockTableModel::sync()
{
    if (m_path == "")
        return false;

    if (m_currentBlock == nullptr)
        return false;

    writeFileData(m_path, m_currentBlock->data());

    return true;
}

bool BlockTableModel::setPath(const QString &path)
{
    beginResetModel();

    QByteArray blkData;

    if (!readFileData(path, blkData))
        return false;

    m_path = path;

    //Driver 2 mission names
    QDir dir(QFileInfo(path).absolutePath());
    dir.cdUp();

    QByteArray missionNames;
    readFileData(dir.path() + "/LANG/EN_MISSION.LTXT", missionNames, true);

    //
    m_driverBlock->clearData();
    m_driver2Block->clearData();

    if (Driver2Block::hasSignature(blkData)) {
        m_blockType = Block::Driver2;
        m_currentBlock = m_driver2Block;

        m_driver2Block->setMissionNames(missionNames);
    }
    else {
        m_blockType = Block::Driver;
        m_currentBlock = m_driverBlock;
    }

    if (!m_currentBlock->setData(blkData)) {
        m_currentBlock = nullptr;

        endResetModel();

        emit error(tr("Can't parse data")
                   + " "
                   + QDir::toNativeSeparators(path));
        return false;
    }

    endResetModel();
    return true;
}

QString BlockTableModel::path() const
{
    return m_path;
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

bool BlockTableModel::exportMission(const QModelIndex &index, const QString &path)
{
    if (!index.isValid())
        return false;

    if (writeFileData(path, m_currentBlock->at(index.row()).data))
        return false;

    return true;
}

bool BlockTableModel::importMission(const QModelIndex &index, const QString &path)
{
    if (!index.isValid())
        return false;

    QByteArray missionData;

    if (!readFileData(path, missionData))
        return false;

    if (!m_currentBlock->setMissionData(index.row(), missionData))
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

bool BlockTableModel::readFileData(const QString &path, QByteArray &buffer, bool silentMode)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        if (!silentMode)
            emit error(tr("Can't open file in read mode")
                   + " "
                   + QDir::toNativeSeparators(path));
        return false;
    }

    buffer = file.readAll();

    file.close();

    return true;
}

bool BlockTableModel::writeFileData(const QString &path, const QByteArray &buffer)
{
    QSaveFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit error(tr("Can't open file in write mode")
                   + " "
                   + QDir::toNativeSeparators(path));
        return false;
    }

    if ((saveFile.write(buffer) == -1)) {
        emit error(tr("Can't write file") +
                   " " +
                   QDir::toNativeSeparators(path));

        return false;
    }

    if (!saveFile.commit()) {
        emit error(tr("Can't commit file") +
                   " " +
                   QDir::toNativeSeparators(path));
        return false;
    }

    return true;
}
