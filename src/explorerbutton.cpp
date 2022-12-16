#include "explorerbutton.h"
#include "gamewindow.h"
#include "itemslot.h"

ExplorerButton::ExplorerButton(int n)
    : n(n),
      m_activity_icon(new QLabel(this)),
      m_couple_status_icon(new QLabel(this)),
      m_portrait_effect(new QGraphicsColorizeEffect(this))
{
    m_item_label->setGraphicsEffect(m_portrait_effect);
    m_item_label->setAlignment(Qt::AlignRight);

    m_activity_icon->setMaximumSize(QSize(16, 16));
    m_activity_icon->setMinimumSize(QSize(16, 16));

    m_couple_status_icon->setMaximumSize(QSize(48, 48));
    m_couple_status_icon->setMinimumSize(QSize(48, 48));
    m_couple_status_icon->setPixmap(Icons::activity_icons().at(Coupling));

    setObjectName(QString("explorer_button;%1").arg(n));
    gw()->register_slot(this);
}

CharacterId ExplorerButton::id() {
    return gw()->game().characters()[n].id();
}

Item ExplorerButton::get_item() {
    return Item();
}

void ExplorerButton::set_item(const Item &) { }

void ExplorerButton::refresh_pixmap() {
    Character &character = gw()->game().characters()[n];

    if (id() == gw()->selected_char_id()) {
        //setFlat(false);
    } else {
        //setFlat(true);
    }

    if (character.dead()) {
        m_portrait_effect->setStrength(0.0);
        m_item_label->setPixmap(QPixmap(":/assets/img/icons/dead_yok.png"));
        m_activity_icon->hide();
        m_couple_status_icon->hide();
        return;
    }

    m_couple_status_icon->setVisible(character.can_couple());
    m_couple_status_icon->raise();

    m_activity_icon->setPixmap(Icons::activity_icons().at(character.activity().action()));
    m_activity_icon->raise();

    m_portrait_effect->setColor(Colors::blend(character.heritage()));

    if (character.id() == NOBODY) {
        m_item_label->setPixmap(QPixmap(":/assets/img/items/empty.png"));
        m_portrait_effect->setColor(QColor(0, 0, 0));
    } else {
        m_item_label->setPixmap(QPixmap(":/assets/img/icons/yok.png"));
    }
}

ItemDomain ExplorerButton::type() {
    return Explorer;
}

void ExplorerButton::insert_explorer_buttons() {
    for (int i = 0; i < MAX_EXPLORERS; i++) {
        gw()->window().explorer_slots->layout()->addWidget(new ExplorerButton(i));
    }
}

void ExplorerButton::mousePressEvent(QMouseEvent *event) {
    Character &character = gw()->game().characters()[n];

    if (id() == NOBODY) {
        return;
    }

    if (event->button() == Qt::LeftButton
        && character.can_couple()
        && character.energy() == character.max_energy()
    ) {
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;

        data->setText(objectName());
        drag->setMimeData(data);
        drag->setPixmap(QPixmap(":/assets/img/icons/yok.png"));

        drag->exec();
    }
}

void ExplorerButton::mouseReleaseEvent(QMouseEvent *) {
    if (id() == NOBODY) {
        return;
    }

    gw()->selected_char_id() = id();
    gw()->refresh_ui();
}

void ExplorerButton::dragEnterEvent(QDragEnterEvent *event) {
    Character &character = gw()->game().characters()[n];

    const QMimeData *data = event->mimeData();
    if (!data->hasFormat("text/plain")) {
        return;
    }

    QString source_slot_name = event->mimeData()->text();
    ItemSlot *source_slot = gw()->findChild<ItemSlot *>(source_slot_name);

    if (character.dead() && source_slot != this) {
        return;
    }

    if (source_slot->type() == Explorer && id() != NOBODY) {
        ExplorerButton *button = (ExplorerButton *) source_slot;
        Character &partner = gw()->game().characters()[button->n];

        if ((partner.dead() && partner.id() != character.id())
            || (character.energy() != character.max_energy())
        ) {
            return;
        }

        event->acceptProposedAction();
    }
}

void ExplorerButton::dropEvent(QDropEvent *event) {
    QString source_slot_name = event->mimeData()->text();
    ExplorerButton *source_button = gw()->findChild<ExplorerButton *>(source_slot_name);

    if (source_button == this) {
        mouseReleaseEvent(nullptr);
        return;
    }

    Character &character = gw()->game().characters()[n];
    Character &partner = gw()->game().characters()[source_button->n];

    character.start_activity(Coupling);
    character.partner() = source_button->id();
    partner.start_activity(Coupling);
    partner.partner() = id();

    partner.can_couple() = false; // i'm a tired
    source_button->refresh_pixmap();
}

bool ExplorerButton::do_hovering() {
    Character &character = gw()->game().characters()[n];

    return character.id() != NOBODY;
}

std::optional<TooltipInfo> ExplorerButton::tooltip_info() {
    Character &character = gw()->game().characters()[n];

    if (character.dead()) {
        return std::optional<TooltipInfo>({
            "<b>" + character.name() + "</b>",
            "Dead Fennahian",
            "<b>This pest has been removed.</b>",
            QPixmap(":/assets/img/icons/dead_yok.png"),
            {},
            std::optional<QColor>()
        });
    }

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
    Character &character = gw()->game().characters()[n];

    QString string;
    switch (character.activity().action()) {
        case Smithing: { string += "<i>Currently smithing</i><br>"; break; }
        case Foraging: { string += "<i>Currently foraging</i><br>"; break; }
        case Mining: { string += "<i>Currently mining</i><br>"; break; }
        case Eating: { string += "<i>Currently eating</i><br>"; break; }
        case Defiling: { string += "<i>Currently defiling</i><br>"; break; }
        case Trading: { string += "<i>Currently trading</i><br>"; break; }
        case Coupling: {
            auto &characters = gw()->game().characters();
            auto partner = std::find_if(begin(characters), end(characters), [&](Character &other) {
                return other.id() == character.partner();
            });
            string += QString("<i>Having a child with %1</i><br>").arg(partner->name());
            break;
        }
        default: { break; }
    }

    if (character.can_couple()) {
        string += "<b><font color=purple>I'm ready to have a child!</font> Drag me onto a partner.</b><br>";
        string += "<b>Both me and my partner must have <font color=red>full energy</font>.</b><br><br>";
    }

    ItemProperties heritage_props = Colors::blend_heritage(character.heritage());

    for (const auto &pair : heritage_props) {
        string += PROPERTY_DESCRIPTIONS.at(pair.first).arg(pair.second) + "<br>";
    }

    return string.left(string.size() - 4);
}
