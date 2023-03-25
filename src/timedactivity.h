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

    virtual void start();
    virtual AorInt percent_complete();
    virtual void complete();
    virtual void update_ui();
    virtual void progress();
    virtual ItemDomain type();

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);

protected:
    AorInt m_ms_total;
};
