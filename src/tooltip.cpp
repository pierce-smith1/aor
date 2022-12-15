#include "tooltip.h"
#include "qnamespace.h"
#include "items.h"
#include "game.h"
#include "gamewindow.h"

Tooltip::Tooltip()
    : widget(),
      m_colorize_effect(new QGraphicsColorizeEffect(this))
{
    widget.setupUi(this);
    setWindowFlags(Qt::ToolTip
        | Qt::WindowTransparentForInput
        | Qt::WindowDoesNotAcceptFocus
    );

    widget.item_image->setGraphicsEffect(m_colorize_effect);
    hide_resources();
}

void Tooltip::set(const TooltipInfo &info) {
    hide_resources();

    widget.item_name->setText(info.title);
    widget.item_subtext->setText(info.subtext);
    widget.item_description->setText(info.description);
    widget.item_image->setPixmap(info.icon);

    for (const auto &pair : info.resource_cost) {
        if (pair.first & Cost) {
            cost_icons().at(pair.first)->show();
            cost_text().at(pair.first)->setText(QString(" %1 ").arg(pair.second));
            cost_text().at(pair.first)->show();
            widget.cost_container->show();
        }
    }

    if (info.colorize.has_value()) {
        m_colorize_effect->setColor(*info.colorize);
        m_colorize_effect->setStrength(1.0);
    } else {
        m_colorize_effect->setStrength(0.0);
    }
}

void Tooltip::set(const Item &item, Game &game) {
    hide_resources();
    m_colorize_effect->setStrength(0.0);

    widget.item_image->setPixmap(Item::pixmap_of(item));

    ItemDefinitionPtr this_def = item.def();
    QString character_name = item.intent_holder == NOBODY ? "" : game.characters().at(item.intent_holder).name();

    widget.item_name->setText(QString("<b>%1</b>").arg(this_def->display_name));
    QString description = this_def->description;
    QString subtext;

    switch (this_def->item_level) {
        case 1: { subtext = "Unremarkable "; break; }
        case 2: { subtext = "Common "; break; }
        case 3: { subtext = "Notable "; break; }
        case 4: { subtext = "Rare "; break; }
        case 5: { subtext = "Enchanted "; break; }
        case 6: { subtext = "Truly Extraordinary "; break; }
        case 7: { subtext = "Anomalous "; break; }
        case 8: { subtext = "Incomprehensible "; break; }
        default: { break; }
    }

    subtext += Item::type_to_string(this_def->type);

    switch (item.intent) {
        default:
        case None: {
            break;
        }
        case Consumable: {
            subtext += QString(" <b><font color=green>(Being eaten by %1)</font></b>").arg(character_name);
            break;
        }
        case Defiling: {
            subtext += QString(" <b><font color=green>(Being defiled by %1)</font></b>").arg(character_name);
            break;
        }
        case Material: {
            subtext += QString(" <b><font color=green>(Queued for smithing by %1)</font></b>").arg(character_name);
            break;
        }
        case Offering: {
            subtext += QString(" <b><font color=green>(Queued for trading)</font></b>");
            break;
        }
        case SmithingTool:
        case ForagingTool:
        case MiningTool:
        case Artifact: {
            subtext += QString(" <b><font color=green>(Equipped by %1)</font></b>").arg(character_name);
            break;
        }
    }

    description += "<br>" + Item::properties_to_string(this_def->properties);

    if (item.uses_left != 0) {
        subtext += QString(" <font color=gray>(%1 uses left)</font>").arg(item.uses_left);
    }

    widget.item_subtext->setText(subtext);
    widget.item_description->setText(description);

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
