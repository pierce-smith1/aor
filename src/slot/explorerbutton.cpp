#include "explorerbutton.h"
#include "../icons.h"

ExplorerButton::ExplorerButton(size_t n)
    : m_activity_icon(new QLabel(this)),
      m_couple_status_icon(new QLabel(this)),
      m_portrait_effect(new QGraphicsColorizeEffect(this)),
      m_n(n)
{
    m_item_label->setGraphicsEffect(m_portrait_effect);
    m_item_label->setAlignment(Qt::AlignRight);

    m_activity_icon->setMaximumSize(16, 16);
    m_activity_icon->setMinimumSize(16, 16);

    m_couple_status_icon->setMaximumSize(48, 48);
    m_couple_status_icon->setMinimumSize(48, 48);
    m_couple_status_icon->setPixmap(Icons::activity_icons().at(Coupling));
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

    if (character().dead() || character().id() == NOBODY) {
        m_portrait_effect->setStrength(0.0);
        m_activity_icon->hide();
        m_couple_status_icon->hide();
        return;
    }

    m_couple_status_icon->setVisible(character().can_couple());
    m_couple_status_icon->raise();

    m_activity_icon->setVisible(true);
    m_activity_icon->setPixmap(Icons::activity_icons().at(character().activity().action()));
    m_activity_icon->raise();

    m_portrait_effect->setStrength(1.0);
    m_portrait_effect->setColor(Colors::blend(character().heritage()));
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

bool ExplorerButton::is_draggable() {
    return character().can_couple()
        && character().energy() == character().max_energy()
        && !character().activity().ongoing();
}

void ExplorerButton::on_left_click(QMouseEvent *) {
    if (character().id() == NOBODY) {
        return;
    }

    gw()->selected_char_id() = character().id();
}

void ExplorerButton::install() {
    QGridLayout *layout = dynamic_cast<QGridLayout *>(gw()->window().explorer_slots->layout());
    layout->addWidget(this, m_n / EXPLORER_COLS, m_n % EXPLORER_COLS);
}

Character &ExplorerButton::character() {
    return gw()->game().characters()[m_n];
}

QString ExplorerButton::character_description() {
    QString string;
    switch (character().activity().action()) {
        case Smithing: { string += "<i>Currently smithing</i><br>"; break; }
        case Foraging: { string += "<i>Currently foraging</i><br>"; break; }
        case Mining: { string += "<i>Currently mining</i><br>"; break; }
        case Eating: { string += "<i>Currently eating</i><br>"; break; }
        case Defiling: { string += "<i>Currently defiling</i><br>"; break; }
        case Trading: { string += "<i>Currently trading</i><br>"; break; }
        case Coupling: {
            Character &partner = gw()->game().character(character().partner());
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

    for (const auto &pair : heritage_props) {
        string += property_definitions().at(pair.first).description.arg(pair.second) + "<br>";
    }

    return string.left(string.size() - 4);
}
