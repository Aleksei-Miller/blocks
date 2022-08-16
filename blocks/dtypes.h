#ifndef DTYPES_H
#define DTYPES_H

#include <QString>

enum Block {
    Unknown = 0,
    Driver = 1,
    Driver2 = 2
};

enum GameMode {
    None = 0,
    Undercover = 1,
    TakeARide= 2,
    Chase = 3,
    Gateway = 4,
    GateRacing = 5,
    Trailblazer = 6,
    Checkpoint = 7,
    Survival = 8,
    SecretRace = 9,
    Replay = 10,
    MultiplayerTakeARide = 11,
    MultiplayerCheckpoint = 12,
    MultiplayerCaptureTheFlag = 13,
    MultiplayerCopsAndRobbers = 14,
    MultiplayerSecretRace = 15,
    DirtTrack = 16,
    Training = 17
};

struct MissionEntry {
    qint32 id;
    QString name;
    qint32 mode;

    QString city;
    QString time;
    QString weather;
    QByteArray data;
};

#endif // DTYPES_H
