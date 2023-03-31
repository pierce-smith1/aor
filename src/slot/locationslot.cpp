#include "locationslot.h"
#include "../icons.h"
#include <QMenu>

LocationSlot::LocationSlot(const LocationDefinition &def, QWidget *parent)
    : m_location_def(def)
{
    setParent(parent);
    setMinimumSize(32, 32);
    setMaximumSize(32, 32);

    m_item_label->setMinimumSize(28, 28);
    m_item_label->setMaximumSize(28, 28);
}

QPixmap LocationSlot::pixmap() {
    if (m_location_def.id & BiomeJungle) {
        return QPixmap(":/assets/img/icons/jungle.png");
    } else if (m_location_def.id & BiomeMesa) {
        return QPixmap(":/assets/img/icons/mesa.png");
    } else {
        return QPixmap(":/assets/img/icons/roses.png");
    }
}

bool LocationSlot::do_hovering() {
    return m_location_def.id != NOWHERE;
}

std::optional<TooltipInfo> LocationSlot::tooltip_info() {
    return std::optional<TooltipInfo>({
        QString("<b>%1</b>").arg(m_location_def.display_name),
        location_subtext(),
        location_description(),
        QPixmap(QString(":/assets/img/items/%1").arg(m_location_def.internal_name)),
        {},
        std::optional<QColor>()
    });
}

void LocationSlot::on_left_click(QMouseEvent *event) {
    QAction *travel_action = new QAction(Icons::activity_icons().at(Travelling), "Travel", this);

    auto travel_check_result = gw()->game()->can_travel(m_location_def.id);
    if (gw()->game()->can_travel(m_location_def.id) != Game::TravelCheckResult::Ok) {
        switch (travel_check_result) {
            case Game::TravelCheckResult::ConcurrentAction: {
                travel_action->setText("Travel (All explorers must be idle)");
                break;
            } case Game::TravelCheckResult::InsufficientResources: {
                travel_action->setText("Travel (Requirements not met)");
                break;
            } case Game::TravelCheckResult::NoPath: {
                travel_action->setText("Travel (Too far away)");
                break;
            } default: {}
        }
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

    if (AorUInt forageables_left = gw()->game()->forageables_left(m_location_def.id)) {
        description += QString("<font color=%2><b>%1</b></font> forageables left here.<br>")
            .arg(forageables_left)
            .arg(Colors::qcolor(Cucumber).name());
    } else {
        description += QString("<b>The land is bare.</b><br>");
    }

    if (AorUInt mineables_left = gw()->game()->mineables_left(m_location_def.id)) {
        description += QString("<font color=%2><b>%1</b></font> mineables left here.<br>")
            .arg(mineables_left)
            .arg(Colors::qcolor(Plum).name());
    } else {
        description += QString("<b>The earth is wasted.</b><br>");
    }

    if (AorUInt signatures_left = gw()->game()->signatures_left(m_location_def.id)) {
        description += QString("<font color=%2><b>%1</b></font> signature items left here.<br>")
            .arg(signatures_left)
            .arg(Colors::qcolor(Cherry).name());
    }

    description += "<br>";
    description += Item::properties_to_string(m_location_def.properties);

    return description;
}

QString LocationSlot::location_subtext() {
    if (m_location_def.id & BiomeJungle) {
        return "Overgrown Vista";
    } else if (m_location_def.id & BiomeMesa) {
        return "Windblown Vista";
    } else if (m_location_def.id & BiomeRoses) {
        return "Rose Vista";
    } else {
        return "Vista";
    }
}
