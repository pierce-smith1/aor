// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "explorerbutton.h"
#include "../icons.h"

ExplorerButton::ExplorerButton(size_t n)
    : m_activity_icon(new QLabel(this)),
      m_couple_status_icon(new QLabel(this)),
      m_status_icons_widget(new QWidget(this)),
      m_tool_status_icon(new QLabel()),
      m_artifact_status_icon(new QLabel()),
      m_injury_status_icon(new QLabel()),
      m_portrait_effect(new QGraphicsColorizeEffect(this)),
      m_n(n)
{
    m_item_label->setGraphicsEffect(m_portrait_effect);
    m_item_label->setAlignment(Qt::AlignRight);

    m_activity_icon->setMaximumSize(16, 16);
    m_activity_icon->setMinimumSize(16, 16);

    m_couple_status_icon->setMaximumSize(16, 16);
    m_couple_status_icon->setMinimumSize(16, 16);
    m_couple_status_icon->setPixmap(Icons::activity_icons().at(Coupling));

    m_status_icons_widget->setLayout(new QHBoxLayout());
    m_status_icons_widget->setMinimumSize(56, 16);
    m_status_icons_widget->setMaximumSize(56, 16);
    m_status_icons_widget->layout()->setSpacing(0);
    m_status_icons_widget->layout()->setContentsMargins(0, 0, 0, 0);

    QLabel *dummy = new QLabel();
    dummy->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    dummy->setMaximumSize(56, 16);
    m_status_icons_widget->layout()->addWidget(dummy);
    m_status_icons_widget->layout()->addWidget(m_tool_status_icon);
    m_status_icons_widget->layout()->addWidget(m_artifact_status_icon);
    m_status_icons_widget->layout()->addWidget(m_injury_status_icon);

    m_tool_status_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_tool_status_icon->setMaximumSize(8, 8);
    m_tool_status_icon->setMinimumSize(8, 8);
    m_tool_status_icon->setPixmap(Icons::explorer_button_icons().at(Tool));

    m_artifact_status_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_artifact_status_icon->setMaximumSize(8, 8);
    m_artifact_status_icon->setMinimumSize(8, 8);
    m_artifact_status_icon->setPixmap(Icons::explorer_button_icons().at(Artifact));

    m_injury_status_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_injury_status_icon->setMaximumSize(12, 8);
    m_injury_status_icon->setMinimumSize(12, 8);
    m_injury_status_icon->setPixmap(Icons::explorer_button_icons().at(Effect));
}

bool ExplorerButton::do_hovering() {
    return character().id() != NOBODY;
}

std::optional<TooltipInfo> ExplorerButton::tooltip_info() {
    if (character().dead()) {
        return std::optional<TooltipInfo>({
            "<b>" + character().name() + "</b>",
            "Dead Fennahian",
            "<b>This pest has been removed.</b>",
            QPixmap(":/assets/img/icons/dead_yok.png"),
            {},
            std::optional<QColor>()
        });
    }

    QString heritage_string;
    int number_of_heritages = 0;
    for (auto it = character().heritage().begin(); it != character().heritage().end();) {
        heritage_string += Colors::name(*it) + ", ";
        std::advance(it, character().heritage().count(*it));
        number_of_heritages++;
    }

    heritage_string = heritage_string.left(heritage_string.size() - 2);

    if (number_of_heritages > 1) {
        heritage_string = "Mixed " + heritage_string;
    }

    return std::optional<TooltipInfo>({
        "<b>" + character().name() + "</b>",
        QString("%1 Fennahian").arg(heritage_string),
        character_description(),
        QPixmap(":/assets/img/icons/yok.png"),
        {},
        std::make_optional(Colors::blend(character().heritage()))
    });
}

void ExplorerButton::refresh() {
    m_item_label->setPixmap(pixmap());

    auto &tools = character().tools();
    m_tool_status_icon->setPixmap(Icons::explorer_button_icons().at(std::any_of(tools.begin(), tools.end(), [](auto &pair) {
        return pair.second != EMPTY_ID;
    }) ? Tool : None));

    auto &artifacts = character().external_items()[Artifact];
    m_artifact_status_icon->setPixmap(Icons::explorer_button_icons().at(std::any_of(artifacts.begin(), artifacts.end(), [](ItemId id) {
        return id != EMPTY_ID;
    }) ? Artifact : None));

    auto &effects = character().effects();
    m_injury_status_icon->setPixmap(Icons::explorer_button_icons().at(std::any_of(effects.begin(), effects.end(), [](Item &effect) {
        return effect.id != EMPTY_ID;
    }) ? Effect : None));

    if (character().dead() || character().id() == NOBODY) {
        m_activity_icon->hide();
        m_couple_status_icon->hide();
        m_portrait_effect->setStrength(0.0);
    } else {
        m_couple_status_icon->setVisible(character().can_couple());
        m_couple_status_icon->raise();

        m_activity_icon->setVisible(true);
        m_activity_icon->setPixmap(Icons::activity_icons().at(character().activity().explorer_subtype()));
        m_activity_icon->raise();

        m_portrait_effect->setStrength(1.0);
        m_portrait_effect->setColor(Colors::blend(character().heritage()));
    }

    if (character().id() == gw()->selected_char_id()) {
        setStyleSheet("*[slot=\"true\"] { border: 2px solid #888; border-radius: 3px; background-color: white; }");
    } else {
        setStyleSheet("*[slot=\"true\"] { border: 1px solid #bbb; border-radius: 3px; background-color: white; }");
    }
}

QPixmap ExplorerButton::pixmap() {
    if (character().id() == NOBODY) {
        return QPixmap(":/assets/img/items/empty.png");
    }

    if (character().dead()) {
        return QPixmap(":/assets/img/icons/dead_yok.png");
    }

    return QPixmap(":/assets/img/icons/yok.png");
}

bool ExplorerButton::will_accept_drop(const SlotMessage &message) {
    if (!std::holds_alternative<CharacterId>(message)) {
        return false;
    }

    if (std::get<CharacterId>(message) == character().id()) {
        return true;
    }

    if (std::get<CharacterId>(message) == NOBODY || character().id() == NOBODY) {
        return false;
    }

    return (character().energy().amount() == character().energy().max(&character()))
        && (character().activity().explorer_subtype() == None);
}

void ExplorerButton::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            if (std::get<CharacterId>(message) == character().id()) {
                on_left_click(nullptr);
                break;
            }

            accept_message(SlotMessage(SlotDoCouple, message, this));
            break;
        } case SlotDoCouple: {
            Character &me = character();
            Character &partner = gw()->game()->character(std::get<CharacterId>(message));

            me.partner() = partner.id();
            me.queue_activity(Coupling, {});
            partner.partner() = me.id();
            partner.queue_activity(Coupling, {});

            partner.can_couple() = false;
            break;
        } default: {}
    }
}

bool ExplorerButton::is_draggable() {
    return character().can_couple()
        && character().energy().amount() == character().energy().max(&character())
        && !character().activity().active;
}

void ExplorerButton::on_left_click(QMouseEvent *) {
    if (character().id() == NOBODY) {
        return;
    }

    gw()->selected_char_id() = character().id();
    CharacterActivity::refresh_ui_bars(character());
}

PayloadVariant ExplorerButton::user_drop_data() {
    return character().id();
}

void ExplorerButton::install() {
    QGridLayout *layout = dynamic_cast<QGridLayout *>(gw()->window().explorer_slots->layout());
    layout->addWidget(this, m_n / EXPLORER_COLS, m_n % EXPLORER_COLS);
}

Character &ExplorerButton::character() {
    return gw()->game()->characters()[m_n];
}

QString ExplorerButton::character_description() {
    QString string;

    string += QString("<b><font color=%4>%2</font></b> spirit, <b><font color=%3>%1</font></b> energy<br><br>")
        .arg(character().energy().amount())
        .arg(character().spirit().amount())
        .arg(Colors::qcolor(Cherry).name())
        .arg(Colors::qcolor(Blueberry).name());

    for (const Item &item : character().equipped_items()) {
        if (item.def()->type & Tool) {
            string += QString("<font color=%2>Holding tool: <b>%1</b></font><br>")
                .arg(item.def()->display_name)
                .arg(Colors::qcolor(Plum).name());
        }

        if (item.def()->type & Artifact) {
            string += QString("<font color=%2>Holding artifact: <b>%1</b></font><br>")
                .arg(item.def()->display_name)
                .arg(Colors::qcolor(Blueberry).name());
        }
    }

    for (const Item &effect : character().nonempty_injuries()) {
        string += QString("<font color=%2>Hurt: <b>%1</b></font><br>")
            .arg(effect.def()->display_name)
            .arg(Colors::qcolor(Cherry).name());
    }

    if (character().nonempty_injuries().size() > 0 || character().equipped_items().size() > 0) {
        string += "<br>";
    }

    switch (character().activity().explorer_subtype()) {
        case Smithing: { string += "<i>Currently smithing</i><br>"; break; }
        case Foraging: { string += "<i>Currently foraging</i><br>"; break; }
        case Mining: { string += "<i>Currently mining</i><br>"; break; }
        case Eating: { string += "<i>Currently eating</i><br>"; break; }
        case Defiling: { string += "<i>Currently defiling</i><br>"; break; }
        case Trading: { string += "<i>Currently trading</i><br>"; break; }
        case Travelling: {
            string += QString("<i>Currently traveling to %1</i><br>")
                .arg(LocationDefinition::get_def(gw()->game()->next_location_id()).display_name);
            break;
        } case Coupling: {
            Character &partner = gw()->game()->character(character().partner());
            string += QString("<i>Having a child with %1</i><br>").arg(partner.name());
            break;
        }
        default: { break; }
    }

    if (character().can_couple()) {
        string += "<b><font color=purple>I'm ready to have a child!</font> Drag me onto a partner.</b><br>";
        string += "<b>Both me and my partner must have <font color=red>full energy</font>.</b><br><br>";
    }

    ItemProperties heritage_props = Colors::blend_heritage(character().heritage());
    string += Item::properties_to_string(heritage_props);

    return string;
}
