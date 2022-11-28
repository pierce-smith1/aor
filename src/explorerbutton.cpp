#include "explorerbutton.h"
#include "gamewindow.h"

ExplorerButton::ExplorerButton(QWidget *parent, LKGameWindow *game_window, CharacterId id)
    : QGroupBox(parent),
      m_game_window(game_window),
      m_id(id),
      m_name(new QLabel(this)),
      m_portrait(new QLabel(this)),
      m_activity_icon(new QLabel(this)),
      m_activity_bar(new QProgressBar(this)),
      m_morale_bar(new QProgressBar(this)),
      m_energy_bar(new QProgressBar(this)),
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
    m_portrait_effect->setColor(m_game_window->game().characters().at(id).color());
    m_portrait->setGraphicsEffect(m_portrait_effect);
    layout->addWidget(m_portrait, 1, 0);

    m_activity_icon->setText("");
    m_activity_icon->setMaximumSize(QSize(16, 16));
    m_activity_icon->setPixmap(QPixmap(":/assets/img/icons/leaf.png"));
    layout->addWidget(m_activity_icon, 1, 1);

    m_activity_bar->setTextVisible(false);
    m_activity_bar->setMaximumSize(QSize(80, 10));
    QPalette activity_palette;
    activity_palette.setColor(QPalette::Highlight, QColor(102, 204, 51));
    m_activity_bar->setPalette(activity_palette);
    layout->addWidget(m_activity_bar, 2, 0, 1, 2);

    m_morale_bar->setTextVisible(false);
    m_morale_bar->setMaximumSize(QSize(80, 10));
    QPalette morale_palette;
    morale_palette.setColor(QPalette::Highlight, QColor(0, 153, 255));
    m_morale_bar->setPalette(morale_palette);
    layout->addWidget(m_morale_bar, 3, 0, 1, 2);

    m_energy_bar->setTextVisible(false);
    m_energy_bar->setMaximumSize(QSize(80, 10));
    QPalette energy_palette;
    energy_palette.setColor(QPalette::Highlight, QColor(255, 51, 0));
    m_energy_bar->setPalette(energy_palette);
    layout->addWidget(m_energy_bar, 4, 0, 1, 2);
}

void ExplorerButton::refresh() {
    Character &character = m_game_window->game().characters().at(m_id);
    m_game_window->game().refresh_ui_bars(m_activity_bar, m_morale_bar, m_energy_bar, m_id);
    m_activity_icon->setPixmap(Icons::activity_icons().at(character.activity().action).pixmap(QSize(16, 16)));

    if (m_id == m_game_window->selected_char_id()) {
        setFlat(false);
    } else {
        setFlat(true);
    }
}

void ExplorerButton::mousePressEvent(QMouseEvent *) {
    m_game_window->selected_char_id() = m_id;
    m_game_window->refresh_ui();
}
