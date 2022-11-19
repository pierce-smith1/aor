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
