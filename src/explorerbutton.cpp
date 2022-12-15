#include "explorerbutton.h"
#include "gamewindow.h"
#include "itemslot.h"

ExplorerButton::ExplorerButton(CharacterId id)
    : m_id(id),
      m_activity_icon(new QLabel(this)),
      m_portrait_effect(new QGraphicsColorizeEffect(this))
{
    m_item_label->setGraphicsEffect(m_portrait_effect);
    m_item_label->setAlignment(Qt::AlignRight);
    m_activity_icon->setGraphicsEffect(nullptr);
    m_activity_icon->setMaximumSize(QSize(16, 16));
    m_activity_icon->setMinimumSize(QSize(16, 16));

    setObjectName(QString("explorer_button;%1").arg(id));
    gw()->register_slot(this);
}

Item ExplorerButton::get_item() {
    return Item();
}

void ExplorerButton::set_item(const Item &) { }

void ExplorerButton::refresh_pixmap() {
    Character &character = gw()->game().characters().at(m_id);

    m_activity_icon->setPixmap(Icons::activity_icons().at(character.activity().action()));
    m_activity_icon->raise();
    m_portrait_effect->setColor(Colors::blend(character.heritage()));
    if (character.id() == NOBODY) {
        m_item_label->setPixmap(QPixmap(":/assets/img/items/empty.png"));
        m_portrait_effect->setColor(QColor(0, 0, 0));
    } else {
        m_item_label->setPixmap(QPixmap(":/assets/img/icons/yok.png"));
    }

    if (m_id == gw()->selected_char_id()) {
        //setFlat(false);
    } else {
        //setFlat(true);
    }
}

void ExplorerButton::insert_explorer_buttons() {
    for (int i = 0; i < MAX_EXPLORERS; i++) {
        gw()->window().explorer_slots->layout()->addWidget(new ExplorerButton(i));
    }
}

void ExplorerButton::mouseReleaseEvent(QMouseEvent *) {
    gw()->selected_char_id() = m_id;
    gw()->refresh_ui();
}

bool ExplorerButton::do_hovering() {
    Character &character = gw()->game().characters().at(m_id);

    return character.id() != NOBODY;
}

std::optional<TooltipInfo> ExplorerButton::tooltip_info() {
    Character &character = gw()->game().characters().at(m_id);

    QString heritage_string;
    int number_of_heritages = 0;
    for (auto it = begin(character.heritage()); it != end(character.heritage());) {
        heritage_string += Colors::name(*it) + ", ";
        std::advance(it, character.heritage().count(*it));
        number_of_heritages++;
    }

    heritage_string = heritage_string.left(heritage_string.size() - 2);

    if (number_of_heritages > 1) {
        heritage_string = "Mixed " + heritage_string;
    }

    return std::optional<TooltipInfo>({
        "<b>" + character.name() + "</b>",
        QString("%1 Fennahian").arg(heritage_string),
        character_description(),
        QPixmap(":/assets/img/icons/yok.png"),
        {},
        std::make_optional(Colors::blend(character.heritage()))
    });
}

QString ExplorerButton::character_description() {
    Character &character = gw()->game().characters().at(m_id);

    QString string;
    switch (character.activity().action()) {
        case Smithing: { string += "<i>Currently smithing</i><br>"; break; }
        case Foraging: { string += "<i>Currently foraging</i><br>"; break; }
        case Mining: { string += "<i>Currently mining</i><br>"; break; }
        case Eating: { string += "<i>Currently eating</i><br>"; break; }
        case Defiling: { string += "<i>Currently defiling</i><br>"; break; }
        case Trading: { string += "<i>Currently trading</i><br>"; break; }
        default: { break; }
    }

    ItemProperties heritage_props = Colors::blend_heritage(character.heritage());

    for (const auto &pair : heritage_props) {
        string += PROPERTY_DESCRIPTIONS.at(pair.first).arg(pair.second) + "<br>";
    }

    return string.left(string.size() - 4);
}
