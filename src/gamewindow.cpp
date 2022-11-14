#include "gamewindow.h"
#include "inventory_ui.h"
#include "cheatconsole.h"
#include "items.h"

CursorItemHolder::CursorItemHolder(LKGameWindow *window)
    : window(window) { }

bool CursorItemHolder::eventFilter(QObject *slot, QEvent *event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (held_item_id == EMPTY_ID) {
                held_item_id = InventoryUi::get_item_in(*window, slot);
                qDebug("Picked up item (id: %lx) (code: %d) (name: %s)",
                    held_item_id,
                    window->get_item_instance(held_item_id)->code,
                    Item::def_of(window->get_item_instance(held_item_id)->code)->internal_name.c_str()
                );
            }
            return true;
        default:
            return false;
    }
}

LKGameWindow::LKGameWindow()
    : window(), item_holder(this)
{
    window.setupUi(this);
    InventoryUi::insert_inventory_slots(*this);
}

void LKGameWindow::refresh_inventory() {
    QMutexLocker lock(&mutex);

    for (int x = 0; x < INVENTORY_COLS; x++) {
        for (int y = 0; y < INVENTORY_ROWS; y++) {
            int item_index = InventoryUi::inventory_index(y, x);
            ItemCode item_code = character.inventory[item_index].code;
            InventoryUi::get_inventory_label(*this, y, x)->setPixmap(Item::pixmap_of(item_code));
        }
    }
}

ItemId LKGameWindow::get_item_id_at(int y, int x) {
    QMutexLocker lock(&mutex);

    return character.inventory[InventoryUi::inventory_index(y, x)].id;
}

Item *LKGameWindow::get_item_instance(ItemId id) {
    auto match_id = [id](Item &item) -> bool { return item.id == id; };
    auto search_result = std::find_if(begin(character.inventory), end(character.inventory), match_id);

    if (search_result == end(character.inventory)) {
        qDebug("Searching for an item by id turned up nothing (%lx)", id);
        return nullptr;
    }

    return &(*search_result);
}

Item *LKGameWindow::get_item_instance_at(int y, int x) {
    return &character.inventory[InventoryUi::inventory_index(y, x)];
}

void LKGameWindow::copy_item_to(const Item &item, int y, int x) {
    if (character.inventory[InventoryUi::inventory_index(y, x)].id != EMPTY_ID) {
        qWarning("Placed an item (id: %lx, code: %d) into a non-empty inventory space (y: %d, x: %d)", item.id, item.code, y, x);
    }

    character.inventory[InventoryUi::inventory_index(y, x)] = item;
}

void LKGameWindow::remove_item_at(int y, int x) {
    character.inventory[InventoryUi::inventory_index(y, x)] = Item();
}

ItemId LKGameWindow::make_item_at(ItemDefinitionPtr def, int y, int x) {
    if (character.inventory[InventoryUi::inventory_index(y, x)].id != EMPTY_ID) {
        qWarning("Made an item (code: %d) at a non-empty inventory space (y: %d, x: %d)", def->code, y, x);
    }

    Item new_item = Item(def);
    character.inventory[InventoryUi::inventory_index(y, x)] = new_item;

    return new_item.id;
}
