#pragma once

#include "../ui_tooltip.h"

struct TooltipText {
    QString title;
    QString subtext;
    QString description;
};

class Tooltip : public QWidget {
public:
    Tooltip();

    void set_text(const TooltipText &text);

    Ui::Tooltip widget;
};
