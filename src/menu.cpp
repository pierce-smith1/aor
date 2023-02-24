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

    QAction *multiwindow_action = new QAction(handbook_menu);
    multiwindow_action->setText("Toggle Multiwindow Mode");
    handbook_menu->addAction(multiwindow_action);
    connect(multiwindow_action, &QAction::triggered, [=]() {
        if (parent->m_multiwindows.empty()) {
            parent->enter_multiwindow_mode();
        } else {
            parent->exit_multiwindow_mode();
        }
    });

    QAction *about_action = new QAction(handbook_menu);
    about_action->setText("About");
    handbook_menu->addAction(about_action);
    connect(about_action, &QAction::triggered, [=]() {
        parent->m_about_box->show();
    });

    addMenu(handbook_menu);
}
