#include "queuedactivityslot.h"
#include "../icons.h"

QueuedActivitySlot::QueuedActivitySlot(size_t n)
    : m_n(n)
{
    setMinimumSize(24, 24);
    setMaximumSize(24, 24);
    m_item_label->setMinimumSize(16, 16);
    m_item_label->setMaximumSize(16, 16);
}

bool QueuedActivitySlot::do_hovering() {
    return activity().id != NO_ACTION;
}

std::optional<TooltipInfo> QueuedActivitySlot::tooltip_info() {
    QString item_name;
    if (activity().explorer_subtype() == Eating || activity().explorer_subtype() == Defiling) {
        Item item = gw()->game()->inventory().get_item(activity().owned_item_ids.at(0));
        bool name_starts_with_vowel = item.def()->display_name.toCaseFolded().startsWith('a')
            || item.def()->display_name.toCaseFolded().startsWith('e')
            || item.def()->display_name.toCaseFolded().startsWith('i')
            || item.def()->display_name.toCaseFolded().startsWith('o')
            || item.def()->display_name.toCaseFolded().startsWith('u');

        item_name = QString(" %1 %2")
            .arg(name_starts_with_vowel ? "an" : "a")
            .arg(item.def()->display_name);
    }

    return std::optional<TooltipInfo>({
        "<b>" + CharacterActivity::domain_to_action_string(activity().explorer_subtype()) + "</b>" + item_name,
        m_n == 0
            ? "Current action"
            : "Queued action",
        m_n == 0
            ? QString("<b>%1</b> is currently doing this.").arg(gw()->selected_char().name())
            : "This action is queued; right click to cancel it.",
        Icons::activity_icons_big().at(activity().explorer_subtype()),
        {},
        std::optional<QColor>()
    });
}

QPixmap QueuedActivitySlot::pixmap() {
    if (activity().id == NO_ACTION) {
        return QPixmap(":assets/img/icons/blank.png");
    } else {
        return Icons::activity_icons().at(activity().explorer_subtype());
    }
}

void QueuedActivitySlot::on_right_click(QMouseEvent *) {
    auto &activities = gw()->selected_char().activities();
    if (m_n > 0 && activity().id != NO_ACTION) {
        TimedActivity &removed_activity = gw()->game()->activity(activities[m_n]);
        for (ItemId id : removed_activity.owned_item_ids) {
            gw()->game()->inventory().get_item_ref(id).owning_action = NO_ACTION;
        }

        activities.erase(begin(activities) + m_n);
    }
}

void QueuedActivitySlot::install() {
    gw()->window().activity_queue_layout->addWidget(this);
}

TimedActivity &QueuedActivitySlot::activity() {
    if (m_n >= gw()->selected_char().activities().size()) {
        return TimedActivity::empty_activity;
    }

    return gw()->game()->activity(gw()->selected_char().activities()[m_n]);
}


