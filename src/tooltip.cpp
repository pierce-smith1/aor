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
    widget.item_name->setText(text.title);
    widget.item_subtext->setText(text.subtext);
    widget.item_description->setText(text.description);
}
