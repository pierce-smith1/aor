#include "locationslot.h"
#include "../icons.h"
#include <QMenu>

LocationSlot::LocationSlot(const LocationDefinition &def, QWidget *parent)
    : m_location_def(def)
{
    setParent(parent);
    setMinimumSize(32, 32);
    setMaximumSize(32, 32);
}

bool LocationSlot::do_hovering() {
    return m_location_def.id != NOWHERE;
}

std::optional<TooltipInfo> LocationSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        QString("<b>%1</b>").arg(m_location_def.display_name),
        QString("Location"),
        location_description(),
        Item::pixmap_of("byteberry"),
        {},
        std::optional<QColor>()
    });
}

void LocationSlot::on_left_click(QMouseEvent *event) {
    QAction *travel_action = new QAction(Icons::activity_icons().at(Travelling), "Travel", this);

    if (!gw()->game()->can_travel(m_location_def.id)) {
        travel_action->setEnabled(false);
    }

    if (travel_action == QMenu::exec({ travel_action }, event->globalPos())) {
        gw()->game()->start_travel(m_location_def.id);
    }
}

void LocationSlot::refresh() {
    Slot::refresh();

    if (m_location_def.id == gw()->game()->current_location_id()) {
        setStyleSheet("*[slot=\"true\"] { border: 2px solid #888; border-radius: 3px; background-color: white; }");
    } else if (m_location_def.id == gw()->game()->next_location_id()) {
        setStyleSheet("*[slot=\"true\"] { border: 2px solid #800; border-radius: 3px; background-color: white; }");
    } else {
        setStyleSheet("*[slot=\"true\"] { border: 1px solid #bbb; border-radius: 3px; background-color: white; }");
    }
}

QString LocationSlot::location_description() {
    QString description = m_location_def.description;

    description += "<br>";

    if (gw()->game()->next_location_id() == m_location_def.id) {
        description += "<i>Our expedition is currently travelling here.</i><br><br>";
    }

    description += QString("<font color=%2><b>%1</b></font> forageables left here.<br>")
        .arg(gw()->game()->forageables_left(m_location_def.id))
        .arg(Colors::qcolor(Cucumber).name());

    description += QString("<font color=%2><b>%1</b></font> mineables left here.")
        .arg(gw()->game()->mineables_left(m_location_def.id))
        .arg(Colors::qcolor(Plum).name());

    description += Item::properties_to_string(m_location_def.properties);

    return description;
}
