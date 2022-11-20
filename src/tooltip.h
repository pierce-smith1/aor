#pragma once

#include "../ui_tooltip.h"

struct TooltipText {
    std::string title;
    std::string subtext;
    std::string description;
};

class Tooltip : public QWidget {
public:
    Tooltip();

    void set_text(const TooltipText &text);

    Ui::Tooltip widget;
};
