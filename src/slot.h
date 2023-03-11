#pragma once

#include <utility>
#include <optional>

#include <QFrame>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsOpacityEffect>

#include "tooltip.h"

using PayloadVariant = std::variant<
    std::monostate,
    ItemId,
    CharacterId,
    ActivityId
>;

const static char PV_EMPTY = 'n';
const static char PV_ITEM = 'i';
const static char PV_CHARACTER = 'c';
const static char PV_ACTIVITY = 'a';

class Slot;

struct DropPayload : public PayloadVariant {
    DropPayload(const PayloadVariant &data, Slot *source);

    // We can only put known MIME types into QDrag objects, so since I don't
    // feel like doing the ritual to register this as a meta type, we just
    // awkwardly serialize it to a string...
    QString to_string() const;
    static DropPayload from_string(const QString &string);

    Slot *source;
};

class Slot : public Hoverable<QFrame> {
public:
    Slot();

    virtual void refresh();
    virtual QPixmap pixmap();
    virtual bool will_accept_drop(const DropPayload &payload);
    virtual bool is_draggable();
    virtual void accept_drop(const DropPayload &payload);
    virtual void after_dropped_elsewhere(const DropPayload &response_payload);
    virtual void on_left_click(QMouseEvent *event);
    virtual void on_right_click(QMouseEvent *event);
    virtual DropPayload get_payload();
    virtual void install();
    virtual ItemDomain type();

protected:
    QLabel *m_item_label;
    QGridLayout *m_item_layout;
    QGraphicsOpacityEffect *m_opacity_effect;

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};
