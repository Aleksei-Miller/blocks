#include "driverblock.h"

DriverBlock::DriverBlock(QObject *parent)
    : QObject{parent}
{
}

QByteArray DriverBlock::data() const
{
    QByteArray rawData;

    QBuffer buffer(&rawData);
    buffer.open(QBuffer::WriteOnly);

    QDataStream dataStream(&buffer);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    //Start offset = 124 * 4(offset) = 496
    quint16 startDataOffset = maxMissions * 4;

    //
    dataStream.device()->seek(0);

    const int missionCount = m_missions.count();
    quint32 offset = startDataOffset;

    //Write header
    for (int i = 0; i < missionCount; i++) {
        dataStream << offset;
        offset += m_missions.at(i).data.size();
    }

    dataStream.device()->seek(startDataOffset);

    //Write data
    for (int i = 0; i < missionCount; i++) {

        if (m_missions.at(i).data.size() == 0)
            continue;

        dataStream.device()->write(m_missions.at(i).data);
    }

    return rawData;
}

bool DriverBlock::setData(const QByteArray &data)
{
    m_missions.clear();

    QDataStream dataStream(data);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    quint32 header[maxMissions];
    dataStream.readRawData(reinterpret_cast<char*>(&header), sizeof(header));

    qint32 offset, length;

    for (int i = 0; i < maxMissions; i++) {

        MissionEntry entry = {};

        entry.id = i;

        if (header[i] > 0) {

            //Offset, length
            offset = header[i];
            length = (i + 1 < maxMissions) ? header[i + 1] - offset : data.size() - offset;

            //
            if (offset > data.size() || length > data.size()) {
                m_missions.clear();
                return false;
            }

            //Read mission data
            dataStream.device()->seek(offset);
            entry.data = dataStream.device()->read(length);

            entry.mode = missionMode(i);
        }

        parseMission(entry);

        m_missions << entry;
    }

    return true;
}

bool DriverBlock::setMissionData(const int id, const QByteArray &data)
{
    if (id < 0 && id > m_missions.count())
        return false;

    MissionEntry &entry = m_missions[id];
    entry.data = data;

    parseMission(entry);

    return true;
}

GameMode DriverBlock::missionMode(const int id) const
{
    if (id > -1 && id < 44)
        return GameMode::Undercover;

    else if (id > 55 && id < 60)
        return GameMode::Gateway;

    else if (id > 59 && id < 68)
        return GameMode::Checkpoint;

    else if (id > 67 && id < 76)
        return GameMode::Trailblazer;

    else if (id > 75 && id < 80)
        return GameMode::Survival;

    else if (id > 79 && id < 88)
        return GameMode::Chase;

    else if (id > 87 && id < 95)
        return GameMode::TakeARide;

    else if (id > 105 && id < 114)
        return GameMode::DirtTrack;

    else if (id == 114 || id == 117)
        return GameMode::Training;

    else if (id > 118 && id < 123)
        return GameMode::Replay;

    return GameMode::None;
}

void DriverBlock::parseMission(MissionEntry &entry)
{
    if (entry.data.size() > 20) {

        QDataStream dataStream(entry.data);
        dataStream.setByteOrder(QDataStream::LittleEndian);

        //Read title
        if (entry.mode == GameMode::Undercover) {
            qint32 signatureStart, signatureEnd;

            while (dataStream.device()->pos() < 128 && !dataStream.atEnd()) {
                dataStream >> signatureStart;

                if (signatureStart == 22) {
                    dataStream >> signatureEnd;

                    if (signatureEnd == 60
                            || signatureEnd == 120
                            || signatureEnd == 150
                            || signatureEnd == 90) {
                        int titleSize;
                        dataStream >> titleSize;

                        if (titleSize < entry.data.size())
                            entry.name = dataStream.device()->read(titleSize);

                        break;
                    }
                }
            }
        }
    }
}
