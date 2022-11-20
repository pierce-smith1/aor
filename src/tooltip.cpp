#include "tooltip.h"
#include "qnamespace.h"

Tooltip::Tooltip()
    : widget()
{
    widget.setupUi(this);
    setWindowFlags(Qt::ToolTip
        | Qt::WindowTransparentForInput
        | Qt::WindowDoesNotAcceptFocus
    );
}

void Tooltip::set_text(const TooltipText &text) {
    widget.item_name->setText(QString::fromStdString(text.title));
    widget.item_subtext->setText(QString::fromStdString(text.subtext));
    widget.item_description->setText(QString::fromStdString(text.description));
}
