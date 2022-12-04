#include "explorerbutton.h"
#include "gamewindow.h"

ExplorerButton::ExplorerButton(QWidget *parent, LKGameWindow *game_window, CharacterId id)
    : QGroupBox(parent),
      m_game_window(game_window),
      m_id(id),
      m_name(new QLabel(this)),
      m_portrait(new QLabel(this)),
      m_activity_icon(new QLabel(this)),
      m_portrait_effect(new QGraphicsColorizeEffect(this))
{
    setObjectName(QString("explorer_button;%1").arg(id));
    setMinimumSize(QSize(80, 16777215));
    setMaximumSize(QSize(120, 140));

    QGridLayout *layout = new QGridLayout(this);

    m_name->setText(QString("<b>%1</b>").arg(m_game_window->game().characters().at(id).name()));
    layout->addWidget(m_name, 0, 0);

    m_portrait->setText("");
    m_portrait->setMaximumSize(QSize(48, 48));
    m_portrait->setPixmap(QPixmap(":/assets/img/icons/yok.png"));
    m_portrait_effect->setColor(COLORS.at(m_game_window->game().characters().at(id).color()));
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

    int i = 0;
    for (ItemDomain domain : { Smithing, Foraging, Mining }) {
        QLabel *label = new QLabel(this);
        label->setText("");
        label->setMinimumSize(QSize(8, 8));
        label->setMaximumSize(QSize(8, 8));
        status_layout->addWidget(label, 3, i++);
        m_tool_status_labels[domain] = label;
    }

    for (size_t n = 0; n < m_artifact_status_labels.size(); n++) {
        QLabel *label = new QLabel(this);
        label->setText("");
        label->setMinimumSize(QSize(8, 8));
        label->setMaximumSize(QSize(8, 8));
        status_layout->addWidget(label, 4, n);
        m_artifact_status_labels[n] = label;
    }
}

void ExplorerButton::refresh() {
    Character &character = m_game_window->game().characters().at(m_id);
    m_activity_icon->setPixmap(Icons::activity_icons().at(character.activity().action));

    if (m_id == m_game_window->selected_char_id()) {
        setFlat(false);
    } else {
        setFlat(true);
    }

    for (ItemDomain domain : { Smithing, Foraging, Mining }) {
        if (character.tools().at(domain) != EMPTY_ID) {
            m_tool_status_labels.at(domain)->setPixmap(Icons::active_status_icons().at(domain));
        } else {
            m_tool_status_labels.at(domain)->setPixmap(Icons::inactive_status_icons().at(domain));
        }
    }

    for (size_t n = 0; n < m_artifact_status_labels.size(); n++) {
        if (character.external_items().at(Artifact).at(n) != EMPTY_ID) {
            m_artifact_status_labels[n]->setPixmap(Icons::active_status_icons().at(Ordinary));
        } else {
            m_artifact_status_labels[n]->setPixmap(Icons::inactive_status_icons().at(Ordinary));
        }
    }
}

void ExplorerButton::mousePressEvent(QMouseEvent *) {
    Character &character = m_game_window->game().characters().at(m_id);

    m_game_window->selected_char_id() = m_id;

    for (int i = 0; i < TRADE_SLOTS; i++) {
        Inventory &inventory = m_game_window->game().inventory();
        m_game_window->connection().offer_changed(inventory.get_item(character.external_items().at(Offering)[i]), i);
    }

    if (character.accepting_trade()) {
        m_game_window->connection().agreement_changed(m_game_window->selected_tribe_id(), true);
    } else {
        m_game_window->connection().agreement_changed(m_game_window->selected_tribe_id(), false);
    }

    m_game_window->refresh_ui();
}
