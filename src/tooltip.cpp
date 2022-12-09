#include "tooltip.h"
#include "qnamespace.h"
#include "items.h"

Tooltip::Tooltip()
    : widget()
{
    widget.setupUi(this);
    setWindowFlags(Qt::ToolTip
        | Qt::WindowTransparentForInput
        | Qt::WindowDoesNotAcceptFocus
    );

    hide_resources();
}

void Tooltip::set_text(const TooltipText &text) {
    widget.item_name->setText(text.title);
    widget.item_subtext->setText(text.subtext);
    widget.item_description->setText(text.description);
}

void Tooltip::set_resources(const Item &item) {
    hide_resources();

    const ItemProperties &properties = item.def()->properties;

    for (const auto &pair : properties) {
        if (pair.first & Resource) {
            icons().at(pair.first)->show();
            text().at(pair.first)->setText(QString(" %1 ").arg(pair.second));
            text().at(pair.first)->show();
            widget.resource_container->show();
        }

        if (pair.first & Cost) {
            cost_icons().at(pair.first)->show();
            cost_text().at(pair.first)->setText(QString(" %1 ").arg(pair.second));
            cost_text().at(pair.first)->show();
            widget.cost_container->show();
        }

        if (pair.first & ToolMaximum) {
            power_icons().at(pair.first)->show();
            power_text().at(pair.first)->setText(QString(" %1 ").arg(pair.second));
            power_text().at(pair.first)->show();
            widget.power_container->show();
        }
    }
}

void Tooltip::hide_resources() {
    for (const auto &pair : icons()) {
        pair.second->hide();
    }

    for (const auto &pair : text()) {
        pair.second->setText("");
    }

    widget.resource_container->hide();

    for (const auto &pair : cost_icons()) {
        pair.second->hide();
    }

    for (const auto &pair : cost_text()) {
        pair.second->setText("");
    }

    widget.cost_container->hide();

    for (const auto &pair : power_icons()) {
        pair.second->hide();
    }

    for (const auto &pair : power_text()) {
        pair.second->setText("");
    }

    widget.power_container->hide();
}

const std::map<quint16, QLabel *> &Tooltip::icons() {
    static std::map<quint16, QLabel *> icons = {
        { StoneResource, widget.stone_icon },
        { MetallicResource, widget.metallic_icon },
        { CrystallineResource, widget.crystal_icon },
        { RunicResource, widget.ruinc_icon },
        { LeafyResource, widget.leafy_icon },
    };

    return icons;
}

const std::map<quint16, QLabel *> &Tooltip::text() {
    static std::map<quint16, QLabel *> text = {
        { StoneResource, widget.stone_text },
        { MetallicResource, widget.metallic_text },
        { CrystallineResource, widget.crystal_text },
        { RunicResource, widget.ruinc_text },
        { LeafyResource, widget.leafy_text },
    };

    return text;
}

const std::map<quint16, QLabel *> &Tooltip::cost_icons() {
    static std::map<quint16, QLabel *> icons = {
        { CostStone, widget.stone_cost_icon },
        { CostMetallic, widget.metallic_cost_icon },
        { CostCrystalline, widget.crystalline_cost_icon },
        { CostRuinc, widget.runic_cost_icon },
        { CostLeafy, widget.leafy_cost_icon },
    };

    return icons;
}

const std::map<quint16, QLabel *> &Tooltip::cost_text() {
    static std::map<quint16, QLabel *> text = {
        { CostStone, widget.stone_cost_text },
        { CostMetallic, widget.metallic_cost_text },
        { CostCrystalline, widget.crystalline_cost_text },
        { CostRuinc, widget.runic_cost_text },
        { CostLeafy, widget.leafy_cost_text },
    };

    return text;
}

const std::map<quint16, QLabel *> &Tooltip::power_icons() {
    static std::map<quint16, QLabel *> icons = {
        { ToolMaximumStone, widget.stone_power_icon },
        { ToolMaximumMetallic, widget.metallic_power_icon },
        { ToolMaximumCrystalline, widget.crystalline_power_icon },
        { ToolMaximumRunic, widget.runic_power_icon },
        { ToolMaximumLeafy, widget.leafy_power_icon },
    };

    return icons;
}

const std::map<quint16, QLabel *> &Tooltip::power_text() {
    static std::map<quint16, QLabel *> text = {
        { ToolMaximumStone, widget.stone_power_text },
        { ToolMaximumMetallic, widget.metallic_power_text },
        { ToolMaximumCrystalline, widget.crystalline_power_text },
        { ToolMaximumRunic, widget.runic_power_text },
        { ToolMaximumLeafy, widget.leafy_power_text },
    };

    return text;
}
