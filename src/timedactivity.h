#pragma once

#include <QTimer>

#include "types.h"
#include "items.h"

const static AorInt ACTIVITY_TICK_RATE_MS = 500;

class TimedActivity : public QTimer {
public:
    TimedActivity(AorInt ms_total, AorInt ms_left);
    TimedActivity(const TimedActivity &other) = delete;
    TimedActivity &operator = (const TimedActivity &other) = delete;
    virtual ~TimedActivity();

    AorInt ms_total();

    virtual double percent_complete();
    virtual void complete();
    virtual ItemDomain domain();

    void serialize(QIODevice *dev) const;
    static TimedActivity *deserialize(QIODevice *dev);

protected:
    AorInt m_ms_total;
};
