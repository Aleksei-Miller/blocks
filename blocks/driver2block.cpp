#include "driver2block.h"

Driver2Block::Driver2Block(QObject *parent)
    : QObject{parent}
{
}

QByteArray Driver2Block::data() const
{
    QByteArray rawData;

    QBuffer buffer(&rawData);
    buffer.open(QBuffer::WriteOnly);

    QDataStream dataStream(&buffer);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    //Start offset = (2 bytes offset + 2 bytes size) * max_missions
    quint16 startDataOffset = maxMissions * 4;

    //
    dataStream.device()->seek(0);

    const int missionCount = m_missions.count();
    quint32 offset = startDataOffset;

    quint32 headerEntry = 0;

    for (int i = 0; i < missionCount; i++) {
        if (m_missions.at(i).data.size() == 0)
            headerEntry = 0;
        else
        {
            headerEntry = m_missions.at(i).data.size() << 19;
            headerEntry |= offset;

            offset += fillingSize(m_missions.at(i).data.size());
        }

        dataStream << headerEntry;
    }

    dataStream.device()->seek(startDataOffset);

    //Write data
    for (int i = 0; i < missionCount; i++) {
        if (m_missions.at(i).data.size() == 0)
            continue;

        dataStream.device()->write(m_missions.at(i).data);

        //Filling
        const int startPos = m_missions.at(i).data.size();
        const int endPos = fillingSize(m_missions.at(i).data.size());

        for (int j = startPos; j < endPos; j++) {
            const char c = '!';
            dataStream.device()->write(&c, 1);
        }
    }

    return rawData;
}

bool Driver2Block::setData(const QByteArray &data)
{
    m_missions.clear();

    QDataStream dataStream(data);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    quint32 header[maxMissions];
    dataStream.readRawData(reinterpret_cast<char*>(&header), sizeof(header));

    qint32 offset, length;

    for (int i = 0; i < maxMissions ; i++) {
        MissionEntry entry = {};

        entry.id = i;

        if (header[i] > 0) {
            offset = header[i] & 0x7ffff;
            length = header[i] >> 19;

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

    if (m_missionNames.count() == 37) {
        for (int i = 1; i < 8; i++) {
            m_missions[i].name = m_missionNames.at(i - 1);
        }
        for (int i = 9; i < 12; i++) {
            m_missions[i].name = m_missionNames.at(i - 2);
        }
        for (int i = 13; i < 36; i++) {
            m_missions[i].name = m_missionNames.at(i - 3);
        }
        for (int i = 37; i < 41; i++) {
            m_missions[i].name = m_missionNames.at(i - 4);
        }
    }

    return true;
}

void Driver2Block::setMissionNames(const QString &names)
{
    m_missionNames.clear();

    QStringList namesList = names.split('\n');

    if (namesList.count() < 50)
        return;

    for (int i = 4; i < 41; i++)
        m_missionNames << namesList.at(i);
}

bool Driver2Block::setMissionData(const int index, const QByteArray &data)
{
    if (index < 0 && index > m_missions.count())
        return false;

    MissionEntry &entry = m_missions[index];
    entry.data = data;

    parseMission(entry);

    return true;
}

int Driver2Block::fillingSize(const int size) const
{
    int result = 0;

    if (size > 0 && size < 2048)
        result = 2048;

    else if (size > 2048 && size < 4096)
        result = 4096;

    else if (size > 4096 && size < 6144)
        result = 6144;

    else if (size > 6144 && size < 8192)
        result = 8192;

    else if (size > 8192 && size < 10240)
        result = 10240;

    return result;
}

void Driver2Block::parseMission(MissionEntry &entry)
{
    QString noneStr = tr("None");

    if (entry.data.size() > 20) {

        QDataStream dataStream(entry.data);
        dataStream.setByteOrder(QDataStream::LittleEndian);

        //Seek offset + id, size
        dataStream.device()->seek(8);

        qint32 value;

        //Read city, time weather
        QStringList cityList;
        cityList << tr("Chicago") << tr("Havana")
                 << tr("Las Vegas") << tr("Rio de Janeiro") ;

        dataStream >> value;

        entry.city = (value > -1 && value < cityList.count())
                ? cityList.at(value) : noneStr;

        QStringList timeList;
        timeList << tr("Day") << tr("Dusk")
                 << tr("Night") << tr("Dawn");

        dataStream >> value;

        entry.time = (value > -1 && value < timeList.count())
                ? timeList.at(value) : noneStr;

        QStringList weatherList;
        weatherList << tr("Clear") << tr("Rainy") << tr("Wet");

        dataStream >> value;

        entry.weather = (value > -1 && value < weatherList.count())
                ? weatherList.at(value) : noneStr;
    }
    else {
        entry.name = noneStr;
        entry.city = noneStr;
        entry.time = noneStr;
        entry.weather = noneStr;
    }
}

GameMode Driver2Block::missionMode(const int id) const
{    
    switch (id) {
    //Undercover
    //Chicago
    case 1: return GameMode::Undercover;
    case 2: return GameMode::Undercover;
    case 3: return GameMode::Undercover;
    case 4: return GameMode::Undercover;
    case 5: return GameMode::Undercover;
    case 6: return GameMode::Undercover;
    case 7: return GameMode::Undercover;
    case 9: return GameMode::Undercover;

        //Havana
    case 10: return GameMode::Undercover;
    case 11: return GameMode::Undercover;
    case 13: return GameMode::Undercover;
    case 14: return GameMode::Undercover;
    case 15: return GameMode::Undercover;
    case 16: return GameMode::Undercover;
    case 17: return GameMode::Undercover;
    case 18: return GameMode::Undercover;
    case 19: return GameMode::Undercover;
    case 20: return GameMode::Undercover;

        //Las-Vegas
    case 21: return GameMode::Undercover;
    case 22: return GameMode::Undercover;
    case 23: return GameMode::Undercover;
    case 24: return GameMode::Undercover;
    case 25: return GameMode::Undercover;
    case 26: return GameMode::Undercover;
    case 27: return GameMode::Undercover;
    case 28: return GameMode::Undercover;
    case 29: return GameMode::Undercover;
    case 30: return GameMode::Undercover;

        //Rio
    case 31: return GameMode::Undercover;
    case 32: return GameMode::Undercover;
    case 33: return GameMode::Undercover;
    case 34: return GameMode::Undercover;
    case 35: return GameMode::Undercover;
    case 37: return GameMode::Undercover;
    case 38: return GameMode::Undercover;
    case 39: return GameMode::Undercover;
    case 40: return GameMode::Undercover;

        //Take a ride
    case 50: return GameMode::TakeARide;
    case 51: return GameMode::TakeARide;
    case 52: return GameMode::TakeARide;
    case 53: return GameMode::TakeARide;
    case 54: return GameMode::TakeARide;
    case 55: return GameMode::TakeARide;
    case 56: return GameMode::TakeARide;
    case 57: return GameMode::TakeARide;

        //Quick chase
    case 70: return GameMode::Chase;
    case 78: return GameMode::Chase;
    case 86: return GameMode::Chase;
    case 94: return GameMode::Chase;

        //Quick gateway
    case 102: return GameMode::Gateway;
    case 103: return GameMode::Gateway;
    case 110: return GameMode::Gateway;
    case 111: return GameMode::Gateway;
    case 118: return GameMode::Gateway;
    case 119: return GameMode::Gateway;
    case 126: return GameMode::Gateway;
    case 127: return GameMode::Gateway;

        //Gate racing
    case 134: return GameMode::GateRacing;
    case 135: return GameMode::GateRacing;
    case 142: return GameMode::GateRacing;
    case 143: return GameMode::GateRacing;
    case 150: return GameMode::GateRacing;
    case 151: return GameMode::GateRacing;
    case 158: return GameMode::GateRacing;
    case 159: return GameMode::GateRacing;

        //Trailblazer
    case 260: return GameMode::Trailblazer;
    case 261: return GameMode::Trailblazer;
    case 268: return GameMode::Trailblazer;
    case 269: return GameMode::Trailblazer;
    case 276: return GameMode::Trailblazer;
    case 277: return GameMode::Trailblazer;
    case 284: return GameMode::Trailblazer;
    case 285: return GameMode::Trailblazer;

        //Checkpoint
    case 196: return GameMode::Checkpoint;
    case 197: return GameMode::Checkpoint;
    case 204: return GameMode::Checkpoint;
    case 205: return GameMode::Checkpoint;
    case 212: return GameMode::Checkpoint;
    case 213: return GameMode::Checkpoint;
    case 220: return GameMode::Checkpoint;
    case 221: return GameMode::Checkpoint;

        //Survival
    case 292: return GameMode::Survival;
    case 300: return GameMode::Survival;
    case 308: return GameMode::Survival;
    case 316: return GameMode::Survival;

        //Replays
    case 400: return GameMode::Replay;
    case 401: return GameMode::Replay;
    case 402: return GameMode::Replay;
    case 403: return GameMode::Replay;

        //Secret race
    case 480: return GameMode::SecretRace;
    case 481: return GameMode::SecretRace;
    case 482: return GameMode::SecretRace;
    case 483: return GameMode::SecretRace;


        //MP  take a ride
    case 58: return GameMode::MultiplayerTakeARide;
    case 59: return GameMode::MultiplayerTakeARide;
    case 60: return GameMode::MultiplayerTakeARide;
    case 61: return GameMode::MultiplayerTakeARide;
    case 62: return GameMode::MultiplayerTakeARide;
    case 63: return GameMode::MultiplayerTakeARide;
    case 64: return GameMode::MultiplayerTakeARide;
    case 65: return GameMode::MultiplayerTakeARide;
    case 498: return GameMode::MultiplayerTakeARide;
    case 499: return GameMode::MultiplayerTakeARide;
    case 500: return GameMode::MultiplayerTakeARide;
    case 501: return GameMode::MultiplayerTakeARide;
    case 502: return GameMode::MultiplayerTakeARide;
    case 503: return GameMode::MultiplayerTakeARide;
    case 504: return GameMode::MultiplayerTakeARide;
    case 505: return GameMode::MultiplayerTakeARide;

        //MP  checkpoint
    case 228: return GameMode::MultiplayerCheckpoint;
    case 229: return GameMode::MultiplayerCheckpoint;
    case 236: return GameMode::MultiplayerCheckpoint;
    case 237: return GameMode::MultiplayerCheckpoint;
    case 244: return GameMode::MultiplayerCheckpoint;
    case 245: return GameMode::MultiplayerCheckpoint;
    case 252: return GameMode::MultiplayerCheckpoint;
    case 253: return GameMode::MultiplayerCheckpoint;

        //MP  capture the flag
    case 352: return GameMode::MultiplayerCaptureTheFlag;
    case 353: return GameMode::MultiplayerCaptureTheFlag;
    case 360: return GameMode::MultiplayerCaptureTheFlag;
    case 361: return GameMode::MultiplayerCaptureTheFlag;
    case 368: return GameMode::MultiplayerCaptureTheFlag;
    case 369: return GameMode::MultiplayerCaptureTheFlag;
    case 376: return GameMode::MultiplayerCaptureTheFlag;
    case 377: return GameMode::MultiplayerCaptureTheFlag;

        //MP  cops'n'robbers
    case 420: return GameMode::MultiplayerCopsAndRobbers;
    case 421: return GameMode::MultiplayerCopsAndRobbers;
    case 428: return GameMode::MultiplayerCopsAndRobbers;
    case 429: return GameMode::MultiplayerCopsAndRobbers;
    case 436: return GameMode::MultiplayerCopsAndRobbers;
    case 437: return GameMode::MultiplayerCopsAndRobbers;
    case 444: return GameMode::MultiplayerCopsAndRobbers;
    case 445: return GameMode::MultiplayerCopsAndRobbers;


        //MP Secret race
    case 484: return GameMode::MultiplayerSecretRace;
    case 485: return GameMode::MultiplayerSecretRace;
    case 486: return GameMode::MultiplayerSecretRace;
    case 487: return GameMode::MultiplayerSecretRace;
    }
    return GameMode::None;
}
