#pragma once

#include <utility>

#include <QFrame>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsOpacityEffect>

#include "gamewindow.h"

const static int INVALID_COORD = -1;

enum SlotType {
    InventorySlot,
    MaterialSlot,
    PrayerSlot,
    ToolSlot,
    ArtifactSlot,
};

class ItemSlot : public QFrame {
    Q_OBJECT

public:
    ItemSlot(LKGameWindow *game);
    ItemSlot(LKGameWindow *game, int y, int x);

    virtual Item get_item();
    virtual void set_item(const Item &item);
    virtual SlotType get_type();
    virtual void refresh_pixmap();
    std::vector<ItemSlot *> get_slots_of_same_type();

    static void insert_inventory_slots(LKGameWindow &window);
    static void insert_inventory_slot(LKGameWindow &window, unsigned y, unsigned x);
    static bool are_yx_coords_oob(int y, int x);
    static size_t inventory_index(int y, int x);
    static QString make_internal_name(const std::string &base, int y, int x);

    int y;
    int x;

protected:
    void enterEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    void drop_external_item();

    QLabel *item_label;
    QLayout *item_layout;
    LKGameWindow *game;

private:
    QGraphicsOpacityEffect opacity_effect;
};

class ExternalSlot : public ItemSlot {
    Q_OBJECT

public:
    ExternalSlot(LKGameWindow *game, SlotType type, int n);

    Item get_item() override;
    void set_item(const Item &item) override;
    SlotType get_type() override;
    void refresh_pixmap() override;

    static void insert_external_slots(LKGameWindow &window);

    SlotType type;
    int n;

protected:
    void dropEvent(QDropEvent *event) override;

private:
    ItemId held_item_id;
};
