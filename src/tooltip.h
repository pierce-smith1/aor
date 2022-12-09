#pragma once

#include "../ui_tooltip.h"

class Item;

struct TooltipText {
    QString title;
    QString subtext;
    QString description;
};

class Tooltip : public QWidget {
public:
    Tooltip();

    void set_text(const TooltipText &item);
    void set_resources(const Item &item);

    Ui::Tooltip widget;

private:
    void hide_resources();
    const std::map<quint16, QLabel *> &icons();
    const std::map<quint16, QLabel *> &text();
    const std::map<quint16, QLabel *> &cost_icons();
    const std::map<quint16, QLabel *> &cost_text();
    const std::map<quint16, QLabel *> &power_icons();
    const std::map<quint16, QLabel *> &power_text();
};
