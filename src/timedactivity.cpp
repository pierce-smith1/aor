#include "timedactivity.h"
#include "gamewindow.h"

TimedActivity::TimedActivity(AorInt ms_total, AorInt ms_left)
    : QTimer(gw()), m_ms_total(ms_total)
{
    gw()->game()->register_activity(this);

    setSingleShot(true);
    setInterval(ms_left);
    callOnTimeout([this]() {
        complete();

        if (m_ms_total != 0) {
            deleteLater();
        }
    });
}

TimedActivity::~TimedActivity() {
    gw()->game()->unregister_activity(this);
}

AorInt TimedActivity::ms_total() {
    return m_ms_total;
}

void TimedActivity::start() {
    QTimer::start();
}

AorInt TimedActivity::percent_complete() {
    if (m_ms_total == 0) {
        return 0;
    }

    return (((m_ms_total - remainingTime()) * 100) / m_ms_total);
}

void TimedActivity::complete() {}

void TimedActivity::update_ui() {}

void TimedActivity::progress() {}

ItemDomain TimedActivity::type() {
    return None;
}

void TimedActivity::serialize(QIODevice *dev) const {
    IO::write_uint(dev, remainingTime() == -1 ? m_ms_total : remainingTime());
    IO::write_uint(dev, m_ms_total);
}

void TimedActivity::deserialize(QIODevice *dev) {
    setInterval(IO::read_uint(dev));
    m_ms_total = IO::read_uint(dev);
}

