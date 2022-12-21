#include "queuedactivityslot.h"
#include "icons.h"
#include "die.h"

QueuedActivitySlot::QueuedActivitySlot(size_t n)
    : n(n)
{
    setMaximumSize(QSize(24, 24));
    setMinimumSize(QSize(24, 24));
    m_item_label->setMaximumSize(QSize(16, 16));
    m_item_label->setMinimumSize(QSize(16, 16));

    gw()->register_slot(this);
}

Item QueuedActivitySlot::get_item() {
    bugcheck(QueuedActivitySlotGet);
    return Item();
}

void QueuedActivitySlot::set_item(const Item &item) {
    bugcheck(QueuedActivitySlotSet, item.code, item.id, item.uses_left, item.intent);
}

void QueuedActivitySlot::refresh_pixmap() {
    if (no_activity()) {
        m_item_label->setPixmap(QPixmap(":assets/img/icons/blank.png"));
    } else {
        Character &character = gw()->selected_char();
        m_item_label->setPixmap(Icons::activity_icons().at(character.activities()[n].action()));
    }
}

void QueuedActivitySlot::insert_queued_activity_slots() {
    QVBoxLayout *layout = gw()->window().activity_queue_layout;

    for (int i = 0; i < MAX_QUEUED_ACTIVITIES; i++) {
        layout->addWidget(new QueuedActivitySlot(i));
    }
}

bool QueuedActivitySlot::do_hovering() {
    return !no_activity();
}

std::optional<Item> QueuedActivitySlot::tooltip_item() {
    return std::optional<Item>();
}

std::optional<TooltipInfo> QueuedActivitySlot::tooltip_info() {
    CharacterActivity &activity = gw()->selected_char().activities()[n];
    QString item_name;
    if (activity.action() == Eating || activity.action() == Defiling) {
        item_name = QString(" a %1")
            .arg(gw()->game().inventory().get_item(activity.owned_items()[0]).def()->display_name);
    }

    return std::optional<TooltipInfo>({
        "<b>" + CharacterActivity::domain_to_action_string(activity.action()) + "</b>" + item_name,
        n == 0
            ? "Current action"
            : "Queued action",
        n == 0
            ? QString("<b>%1</b> is currently doing this.").arg(gw()->selected_char().name())
            : "This action is queued; right click to cancel it.",
        Icons::activity_icons_big().at(activity.action()),
        {},
        std::optional<QColor>()
    });
}

void QueuedActivitySlot::mousePressEvent(QMouseEvent *event) {
    Activities &activities = gw()->selected_char().activities();
    if (event->button() == Qt::RightButton && n > 0 && !no_activity()) {
        CharacterActivity &removed_activity = activities[n];
        for (ItemId id : removed_activity.owned_items()) {
            gw()->game().inventory().get_item_ref(id).intent = None;
            gw()->game().inventory().get_item_ref(id).owning_action = NO_ACTION;
        }

        activities.erase(begin(activities) + n);
        gw()->refresh_slots();
    }
};

void QueuedActivitySlot::dragEnterEvent(QDragEnterEvent *) { };

bool QueuedActivitySlot::no_activity() {
    Character &character = gw()->selected_char();
    return n >= character.activities().size();
}
