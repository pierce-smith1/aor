#pragma once

#include <QObject>
#include <QRecursiveMutex>

class LKGameWindow;

#include "items.h"
#include "state.h"
#include "../ui_main.h"

class CursorItemHolder : public QObject {
    Q_OBJECT

public:
    CursorItemHolder(LKGameWindow *window);

protected:
    bool eventFilter(QObject *slot, QEvent *event) override;

private:
    ItemId held_item_id;
    LKGameWindow *window;
};

class LKGameWindow : public QMainWindow {
public:
    LKGameWindow();

    void refresh_inventory();
    ItemId get_item_id_at(int y, int x);
    Item *get_item_instance(ItemId id);
    Item *get_item_instance_at(int y, int x);
    void copy_item_to(const Item &item, int y, int x);
    void remove_item_at(int y, int x);
    ItemId make_item_at(ItemDefinitionPtr def, int y, int x);

    Ui::LKMainWindow window;
    CharacterState character;
private:
    CursorItemHolder item_holder;
    QRecursiveMutex mutex;
};
