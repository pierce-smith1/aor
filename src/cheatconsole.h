#pragma once

#include <algorithm>

#include <QWidget>
#include <QString>

class CheatConsole;

#include "gamewindow.h"
#include "itemslot.h"
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
if (Inventory::are_yx_coords_oob(y, x)) { \
    qWarning("Yx coords were oob (y: %d, x: %d)", y, x); \
    return; \
}

using CheatAction = std::function<void(LKGameWindow *game, const QStringList &args)>;

struct CheatCommand {
    QString name;
    QString help;
    int nargs;
    CheatAction action;
};

const static std::vector<CheatCommand> COMMANDS = {
    {
        "help",
        "Display the help text for a command",
        1,
        [](LKGameWindow *, const QStringList &args) {
            QString command_name = args[0];
            auto match_name = [&command_name](const CheatCommand command) -> bool {
                return command.name == command_name;
            };

            auto command = std::find_if(begin(COMMANDS), end(COMMANDS), match_name);
            if (command == end(COMMANDS)) {
                qWarning("Failed to find command for printing help (%s)", command_name.toStdString().c_str());
                return;
            }

            qDebug("help: %s", command->help.toStdString().c_str());
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

            Item item = game->game().inventory().get_item(y, x);

            qDebug("print: item yx (%d, %d): code (%d), id (%llx), name (%s), intent (%d)",
                y, x,
                item.code,
                item.id,
                Item::def_of(item.code)->internal_name.toStdString().c_str(),
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

            Item item {game->game().inventory().get_item(y0, x0)};
            game->game().inventory().put_item(item, y1, x1);
            game->game().inventory().remove_item(y0, x0);

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

            Item item_a {game->game().inventory().get_item(y0, x0)};
            Item item_b {game->game().inventory().get_item(y1, x1)};

            game->game().inventory().put_item(item_a, y1, x1);
            game->game().inventory().put_item(item_b, y0, x0);

            qDebug("done");
        }
    },
    {
        "make",
        "Make a new item with name ($0) at yx ($1, $2)",
        3,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(y, args[1]);
            QS_TO_INT(x, args[2]);

            OOB_CHECK(y, x);

            game->game().inventory().put_item(Item(Item::def_of(args[0])), y, x);

            qDebug("done");
        }
    },
    {
        "save",
        "Save the current state of the game to the file \"save.lk\"",
        0,
        [](LKGameWindow *game, const QStringList &) {
            game->save();

            qDebug("done");
        }
    },
    {
        "load",
        "Load a saved character state from the file \"save.lk\"",
        0,
        [](LKGameWindow *game, const QStringList &) {
            game->load();

            qDebug("done");
        }
    },
    {
        "name",
        "Change the current character's name to ($0); if $0 is '?', show the current name instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            QString new_name = args[0];
            if (new_name == "?") {
                qDebug("My name is (%s)", game->selected_char().name().toStdString().c_str());
                return;
            }

            game->selected_char().name() = new_name;

            qDebug("done");
        }
    },
    {
        "energy",
        "Change the current character's energy to ($0); if $0 is '?', show the current energy instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current energy is (%d)", game->selected_char().energy());
                return;
            }

            QS_TO_INT(energy, args[0]);

            if (energy < 0 || energy > 100) {
                qWarning("New energy value is oob (%d)", energy);
                return;
            }

            game->selected_char().energy() = energy;

            qDebug("done");
        }
    },
    {
        "morale",
        "Change the current character's morale to ($0); if $0 is '?', show the current morale instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current morale is (%d)", game->selected_char().morale());
                return;
            }

            QS_TO_INT(morale, args[0]);

            if (morale < 0 || morale > 100) {
                qWarning("New morale value is oob (%d)", morale);
                return;
            }

            game->selected_char().morale() = morale;

            qDebug("done");
        }
    },
    {
        "start",
        "Start doing activity ($0) with ($1) ms left, where $0 is the activity enum",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(action, args[0]);
            QS_TO_INT(ms, args[1]);

            game->start_activity({ (ItemDomain) action, ms });

            qDebug("done");
        }
    },
    {
        "timel",
        "Change the amount of ms left in the current activity to ($0); if $0 is '?', show the amount of ms left instead",
        1,
        [](LKGameWindow *game, const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Amount of ms left in activity is (%ld)", game->selected_char().activity().ms_left);
                return;
            }

            QS_TO_INT(ms, args[0]);

            game->selected_char().activity().ms_left = ms;

            qDebug("done");
        }
    },
    {
        "effect",
        "Set the effect at n = ($0) to a new item with code ($1)",
        2,
        [](LKGameWindow *game, const QStringList &args) {
            QS_TO_INT(n, args[0]);
            QS_TO_INT(code, args[1]);

            game->selected_char().effects()[n] = Item(code);

            qDebug("done");
        }
    }
};

class CheatConsole : public QWidget {
    Q_OBJECT;

public:
    CheatConsole(LKGameWindow *window);

    void execute(const QString &input);

    LKGameWindow *game;
    Ui::CheatWindow console;

public slots:
    void consume_input();
};
