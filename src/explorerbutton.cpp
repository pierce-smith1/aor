#include "explorerbutton.h"
#include "gamewindow.h"
#include "itemslot.h"

ExplorerButton::ExplorerButton(QWidget *parent, CharacterId id)
    : Hoverable(gw()->tooltip(), parent),
      m_id(id),
      m_name(new QLabel(this)),
      m_portrait(new QLabel(this)),
      m_activity_icon(new QLabel(this)),
      m_portrait_effect(new QGraphicsColorizeEffect(this))
{
    setMouseTracking(true);

    setObjectName(QString("explorer_button;%1").arg(id));
    setMinimumSize(QSize(80, 16777215));
    setMaximumSize(QSize(120, 140));

    QGridLayout *layout = new QGridLayout(this);

    m_name->setText(QString("<b>%1</b>").arg(gw()->game().characters().at(id).name()));
    layout->addWidget(m_name, 0, 0);

    m_portrait->setText("");
    m_portrait->setMaximumSize(QSize(48, 48));
    m_portrait->setPixmap(QPixmap(":/assets/img/icons/yok.png"));
    m_portrait_effect->setColor(Colors::qcolor((gw()->game().characters().at(id).color())));
    m_portrait->setGraphicsEffect(m_portrait_effect);
    layout->addWidget(m_portrait, 1, 0);

    m_activity_icon->setText("");
    m_activity_icon->setMaximumSize(QSize(16, 16));
    m_activity_icon->setPixmap(QPixmap(":/assets/img/icons/leaf.png"));
    layout->addWidget(m_activity_icon, 1, 1);

    QWidget *status_area = new QWidget(this);
    QGridLayout *status_layout = new QGridLayout(status_area);
    status_layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(status_area, 2, 0);
}

void ExplorerButton::refresh() {
    Character &character = gw()->game().characters().at(m_id);
    m_activity_icon->setPixmap(Icons::activity_icons().at(character.activity().action()));

    if (m_id == gw()->selected_char_id()) {
        setFlat(false);
    } else {
        setFlat(true);
    }
}

void ExplorerButton::mousePressEvent(QMouseEvent *) {
    gw()->selected_char_id() = m_id;
    gw()->refresh_ui();
}

bool ExplorerButton::do_hovering() {
    return true;
}

std::optional<TooltipInfo> ExplorerButton::tooltip_info() {
    Character &character = gw()->game().characters().at(m_id);
    return std::optional<TooltipInfo>({
        character.name(),
        QString("%1 Fennahian").arg(Colors::name(character.color())),
        "gnocklin",
        QPixmap(":/assets/img/icons/yok.png")
    });
}

