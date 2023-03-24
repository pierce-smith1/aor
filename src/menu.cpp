#include "menu.h"
#include "gamewindow.h"
#include "encyclopedia.h"
#include "about.h"

MenuBar::MenuBar(LKGameWindow *parent)
    : QMenuBar(parent)
{
    QMenu *handbook_menu = new QMenu("Handbook", this);

    QAction *encyclopedia_action = new QAction(handbook_menu);
    encyclopedia_action->setText("Encyclopedia");
    handbook_menu->addAction(encyclopedia_action);
    connect(encyclopedia_action, &QAction::triggered, [=]() {
        parent->m_encyclopedia->show();
    });

    QAction *log_action = new QAction(handbook_menu);
    log_action->setText("Event Log");
    handbook_menu->addAction(log_action);
    connect(log_action, &QAction::triggered, [=]() {
        ((QWidget *) parent->m_event_log.events_list->parent())->show();
    });

    QAction *seperator = new QAction(handbook_menu);
    seperator->setSeparator(true);
    handbook_menu->addAction(seperator);

    QAction *about_action = new QAction(handbook_menu);
    about_action->setText("About");
    handbook_menu->addAction(about_action);
    connect(about_action, &QAction::triggered, [=]() {
        parent->m_about_box->show();
    });

    addMenu(handbook_menu);

    QMenu *settings_menu = new QMenu("Settings", this);

    QAction *multiwindow_action = new QAction(settings_menu);
    multiwindow_action->setCheckable(true);
    multiwindow_action->setChecked(gw()->game()->settings().multiwindow_on);
    multiwindow_action->setText("Multiwindow");
    settings_menu->addAction(multiwindow_action);
    connect(multiwindow_action, &QAction::triggered, [=]() {
        gw()->game()->settings().multiwindow_on = multiwindow_action->isChecked();

        if (multiwindow_action->isChecked()) {
            gw()->enter_multiwindow_mode();
        } else {
            gw()->exit_multiwindow_mode();
        }
    });

    QAction *sound_action = new QAction(settings_menu);
    sound_action->setCheckable(true);
    sound_action->setChecked(gw()->game()->settings().sounds_on);
    sound_action->setText("Sounds");
    settings_menu->addAction(sound_action);
    connect(sound_action, &QAction::triggered, [=]() {
        gw()->game()->settings().sounds_on = sound_action->isChecked();
    });

    addMenu(settings_menu);
}
