#include "timedactivity.h"
#include "gamewindow.h"

TimedActivity::TimedActivity(AorInt ms_total, AorInt ms_left)
    : QTimer(gw()), m_ms_total(ms_total)
{
    gw()->game().register_activity(domain(), this);

    setSingleShot(true);
    setInterval(ms_left);
    callOnTimeout([this](){ complete(); });
}

TimedActivity::~TimedActivity() {
    gw()->game().unregister_activity(this);
}

AorInt TimedActivity::ms_total() {
    return m_ms_total;
}

double TimedActivity::percent_complete() {
    if (m_ms_total == 0) {
        return 0.0;
    }

    return ((double) (m_ms_total - remainingTime()) / m_ms_total);
}

void TimedActivity::complete() {
    gw()->game().unregister_activity(this);
}

ItemDomain TimedActivity::domain() {
    return None;
}

void TimedActivity::serialize(QIODevice *dev) const {
    IO::write_uint(dev, remainingTime());
    IO::write_uint(dev, m_ms_total);
}

TimedActivity *TimedActivity::deserialize(QIODevice *dev) {
    TimedActivity *a = new TimedActivity(0, 0);

    a->setInterval(IO::read_uint(dev));
    a->m_ms_total = IO::read_uint(dev);

    return a;
}

