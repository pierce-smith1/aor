#pragma once

#include <QEnterEvent>
#include <QMouseEvent>
#include <QGraphicsColorizeEffect>

#include <optional>

#include "../ui_tooltip.h"
#include "items.h"
#include "gamewindow.h"
#include "sounds.h"

class Game;

struct TooltipInfo {
    QString title;
    QString subtext;
    QString description;
    QPixmap icon;
    ItemProperties resource_cost;
    std::optional<QColor> colorize = std::make_optional<QColor>();
};

class Tooltip : public QWidget {
public:
    Tooltip();

    void set(const TooltipInfo &item);
    void set(const Item &item);
    void color_cost_text();

    Ui::Tooltip widget;

private:
    void hide_resources();
    const std::map<AorUInt, QLabel *> &icons();
    const std::map<AorUInt, QLabel *> &text();
    const std::map<AorUInt, QLabel *> &cost_icons();
    const std::map<AorUInt, QLabel *> &cost_text();
    const std::map<AorUInt, QLabel *> &power_icons();
    const std::map<AorUInt, QLabel *> &power_text();

    QGraphicsColorizeEffect *m_colorize_effect;
};

template<
    typename T,
    typename = std::enable_if_t<std::is_constructible<T, QWidget *>::value>
> class Hoverable : public T {
public:
    Hoverable(Tooltip *tooltip, QWidget *parent)
        : T(parent),
          m_tooltip(tooltip) { }

protected:
    virtual void enterEvent(QEvent *event) {
        if (!do_hovering()) {
            return;
        }

        if (gw()->game()->settings().sounds_on) {
            Sounds::hover_sound()->play();
        }

        QEnterEvent *enter_event = (QEnterEvent *) event;
        m_tooltip->move(enter_event->globalPos());

        if (tooltip_info()) {
            TooltipInfo info = *tooltip_info();
            m_tooltip->set(info);
        } else {
            Item item = tooltip_item().value();
            m_tooltip->set(item);
        }

        m_tooltip->adjustSize();
        m_tooltip->show();
    }

    virtual void mouseMoveEvent(QMouseEvent *event) {
        if (do_hovering()) {
            m_tooltip->move(event->globalPos());
        }
    }

    virtual void leaveEvent(QEvent *) {
        m_tooltip->hide();
    }

    virtual bool do_hovering() {
        return false;
    }

    virtual std::optional<TooltipInfo> tooltip_info() {
        return std::optional<TooltipInfo>();
    }

    virtual std::optional<Item> tooltip_item() {
        return std::optional<Item>();
    }

    Tooltip *m_tooltip;
};
