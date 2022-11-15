#pragma once

#include <algorithm>

#include <QWidget>
#include <QString>

class CheatConsole;

#include "gamewindow.h"
#include "inventory_ui.h"
#include "../ui_cheat.h"

#define QS_TO_INT(i, s) \
int i; \
{ \
    bool ok; \
    i = s.toInt(&ok); \
    if (!ok) { \
        qWarning("Expected a number for argument (%s)", s.toStdString().c_str()); \
        return; \
    } \
}

#define OOB_CHECK(y, x) \
if (InventoryUi::are_yx_coords_oob(y, x)) { \
    qWarning("Yx coords were oob (y: %d, x: %d)", y, x); \
    return; \
}

using CheatAction = std::function<void(LKGameWindow *game, const QStringList &args)>;

struct CheatCommand {
    std::string name;
    std::string help;
    int nargs;
    CheatAction action;
};

const static std::vector<CheatCommand> COMMANDS = {
    {
        "help",
        "Display the help text for a command",
        1,
        [](LKGameWindow *, const QStringList &args) {
            std::string command_name = args[0].toStdString();
            auto match_name = [&command_name](const CheatCommand command) -> bool { return command.name == command_name; };

            auto command = std::find_if(begin(COMMANDS), end(COMMANDS), match_name);
            if (command == end(COMMANDS)) {
                qWarning("Failed to find command for printing help (%s)", command_name.c_str());
                return;
            }

            qDebug("help: %s", command->help.c_str());
        }
    },
    {
        "print",
        "Print detailed information about the item at yx ($0, $1) in the inventory",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(y, args[0])
            QS_TO_INT(x, args[1])

            OOB_CHECK(y, x);

            Item item = game->get_item_instance_at(y, x);
            qDebug("print: item yx (%d, %d): code (%d), id (%lx), name (%s), intent (%d)",
                y, x,
                item.code,
                item.id,
                Item::def_of(item.code)->internal_name.c_str(),
                item.intent
            );
        }
    },
    {
        "move",
        "Move the item at yx ($0, $1) in the inventory to yx ($2, $3) in the inventory",
        4,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(y0, args[0]);
            QS_TO_INT(x0, args[1]);
            QS_TO_INT(y1, args[2]);
            QS_TO_INT(x1, args[3]);

            OOB_CHECK(y0, x0);
            OOB_CHECK(y1, x1);

            Item item = game->get_item_instance_at(y0, x0);
            game->copy_item_to(item, y1, x1);
            game->remove_item_at(y0, x0);

            game->refresh_inventory();

            qDebug("done");
        }
    },
    {
        "swap",
        "Swap the item at yx ($0, $1) in the inventory with the item at yx ($2, $3) in the inventory",
        4,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(y0, args[0]);
            QS_TO_INT(x0, args[1]);
            QS_TO_INT(y1, args[2]);
            QS_TO_INT(x1, args[3]);

            OOB_CHECK(y0, x0);
            OOB_CHECK(y1, x1);

            Item item_a(game->get_item_instance_at(y0, x0));
            Item item_b(game->get_item_instance_at(y1, x1));

            game->copy_item_to(item_a, y1, x1);
            game->copy_item_to(item_b, y0, x0);

            game->refresh_inventory();

            qDebug("done");
        }
    },
    {
        "make",
        "Make a new item with code ($0) at yx ($1, $2)",
        3,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(code, args[0]);
            QS_TO_INT(y, args[1]);
            QS_TO_INT(x, args[2]);

            OOB_CHECK(y, x);

            game->make_item_at(Item::def_of(code), y, x);

            game->refresh_inventory();

            qDebug("done");
        }
    },
    {
        "save",
        "Save the current state of the game to the file ($0)",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            StateSerialize::save_state(game->character, args[0].toStdString());

            qDebug("done");
        }
    },
    {
        "load",
        "Load a saved character state from the file ($0)",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            CharacterState *state = StateSerialize::load_state(args[0].toStdString());

            if (state == nullptr) {
                qWarning("State file is not proper!");
                return;
            }

            game->character = *state;
            delete state;

            qDebug("done");
        }
    },
    {
        "name",
        "Change the current character's name to ($0); if $0 is '?', show the current name instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            std::string new_name = args[0].toStdString();
            if (new_name == "?") {
                qDebug("My name is (%s)", game->character.name.c_str());
                return;
            }

            game->character.name = new_name;

            qDebug("done");
        }
    },
    {
        "hunger",
        "Change the current character's hunger to ($0); if $0 is '?', show the current hunger instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current hunger is (%d)", game->character.hunger);
                return;
            }

            QS_TO_INT(hunger, args[0]);

            if (hunger < 0 || hunger > 100) {
                qWarning("New hunger value is oob (%d)", hunger);
                return;
            }

            game->character.hunger = hunger;

            qDebug("done");
        }
    }
};

class CheatConsole : QWidget {
    Q_OBJECT;

public:
    CheatConsole(LKGameWindow *window);

    void execute(const std::string &input);

    LKGameWindow *game;
    Ui::CheatWindow console;

public slots:
    void consume_input();
};
