#ifndef DRIVER2BLOCK_H
#define DRIVER2BLOCK_H

#include <QObject>
#include <QVector>

#include <QBuffer>
#include <QDataStream>

#include <QDebug>

#include "abstractblock.h"
#include "dtypes.h"

class Driver2Block : public QObject, public AbstractBlock
{
    Q_OBJECT
public:
    explicit Driver2Block(QObject *parent = nullptr);

    QByteArray data() const override;
    bool setData(const QByteArray &data) override;

    bool setMissionData(const int id, const QByteArray &data) override;
    void setMissionNames(const QString &names = "");

    static bool hasSignature(const QByteArray &data)
    {
        return data.contains("SM2D");
    }

signals:

private:
    const qint16 maxMissions = 512;

    QStringList m_missionNames;

    int fillingSize(const int size) const;
    void parseMission(MissionEntry &entry);
    GameMode missionMode(const int id) const;
};

#endif // DRIVER2BLOCK_H
