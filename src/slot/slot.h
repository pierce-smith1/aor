// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

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

#include "../tooltip.h"

using PayloadVariant = std::variant<
    std::monostate,
    Item,
    CharacterId,
    ActivityId
>;

const static char PV_EMPTY = 'n';
const static char PV_ITEM = 'i';
const static char PV_CHARACTERID = 'c';
const static char PV_ACTIVITYID = 'a';

enum SlotMessageType {
    SlotUserDrop,
    SlotSetItem,
    SlotForgetItem,
    SlotDoCouple,
};

class Slot;

struct SlotMessage : public PayloadVariant {
    SlotMessage(SlotMessageType type, const PayloadVariant &data, Slot *source);

    // We can only put known MIME types into QDrag objects, so since I don't
    // feel like doing the ritual to register this as a meta type, we just
    // awkwardly serialize it to a string...
    QString to_data_string() const;
    static SlotMessage from_data_string(const QString &string);

    Slot *source;
    SlotMessageType type;
};

class Slot : public Hoverable<QFrame> {
public:
    Slot();

    virtual void refresh();
    virtual QPixmap pixmap();
    virtual bool will_accept_drop(const SlotMessage &message);
    virtual bool is_draggable();
    virtual void accept_message(const SlotMessage &message);
    virtual void on_left_click(QMouseEvent *event);
    virtual void on_right_click(QMouseEvent *event);
    virtual PayloadVariant user_drop_data();
    virtual void install();

protected:
    QLabel *m_item_label;
    QGridLayout *m_item_layout;
    QGraphicsOpacityEffect *m_opacity_effect;

    void make_wide();
    void make_tall();

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};
