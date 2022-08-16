#ifndef DRIVERBLOCK_H
#define DRIVERBLOCK_H

#include <QObject>
#include <QVector>

#include <QDataStream>
#include <QBuffer>

#include "abstractblock.h"
#include "dtypes.h"

class DriverBlock : public QObject, public AbstractBlock
{
    Q_OBJECT

public:
    explicit DriverBlock(QObject *parent = nullptr);

    QByteArray data() const override;
    bool setData(const QByteArray &data) override;

    bool setMissionData(const int id, const QByteArray &data) override;

signals:

private:
    const qint16 maxMissions = 124;

    QStringList m_gameModeList;

    GameMode missionMode(const int id) const;
    void parseMission(MissionEntry &entry);
};

#endif // DRIVERBLOCK_H
