#ifndef ABSTRACTBLOCK_H
#define ABSTRACTBLOCK_H

#include <QByteArray>
#include <QStringList>
#include <QVector>

#include "dtypes.h"

class AbstractBlock
{
public:
    virtual ~AbstractBlock(){}

    virtual  QByteArray data() const = 0;
    virtual bool setData(const QByteArray &data) = 0;
    virtual bool setMissionData(const int id, const QByteArray &data) = 0;

    const QByteArray &missionData(const int id) const
    {
        return m_missions.at(id).data;
    }

    const MissionEntry &at(int id)
    {
        return m_missions.at(id);
    }

    int count() const
    {
        return m_missions.count();
    }

    void clearData()
    {
        m_missions.clear();
    }

protected:
    QVector<MissionEntry> m_missions;
};

#endif // ABSTRACTBLOCK_H
