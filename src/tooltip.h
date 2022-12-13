#pragma once

#include <QEnterEvent>
#include <QMouseEvent>
#include <optional>

#include "../ui_tooltip.h"
#include "items.h"
#include "gamewindow.h"

class Game;

struct TooltipInfo {
    QString title;
    QString subtext;
    QString description;
    QPixmap icon;
};

class Tooltip : public QWidget {
public:
    Tooltip();

    void set(const TooltipInfo &item);
    void set(const Item &item, Game &game);

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

        QEnterEvent *enter_event = (QEnterEvent *) event;
        m_tooltip->move(enter_event->globalPos());

        if (tooltip_info()) {
            TooltipInfo info = *tooltip_info();
            m_tooltip->set(info);
        } else {
            Item item = tooltip_item().value();
            m_tooltip->set(item, gw()->game());
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
