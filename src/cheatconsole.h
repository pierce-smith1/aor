#pragma once

#include <algorithm>

#include <QWidget>
#include <QString>

class CheatConsole;

#include "gamewindow.h"
#include "slot/itemslot.h"
#include "../ui_cheat.h"

#define QS_TO_INT(i, s) \
AorInt i; \
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
    qWarning("Yx coords were oob (y: %lld, x: %lld)", y, x); \
    return; \
}

using CheatAction = std::function<void(const QStringList &args)>;

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
        [](const QStringList &args) {
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
        "Print detailed information about the item at yx $0, $1 in the inventory",
        2,
        [](const QStringList &args) {
            QS_TO_INT(y, args[0])
            QS_TO_INT(x, args[1])

            OOB_CHECK(y, x);

            Item item = gw()->game()->inventory().get_item(y, x);

            qDebug("print: item yx (%lld, %lld): code (%llx), id (%llx), name (%s)",
                y, x,
                item.code.n,
                item.id.n,
                Item::def_of(item.code)->internal_name.toStdString().c_str()
            );
        }
    },
    {
        "make",
        "Make a new item with name $0 at yx $1, $2; if $1 and $2 are '.', then make it in the first available slot",
        3,
        [](const QStringList &args) {
            if (args[1] == "." && args[2] == ".") {
                gw()->game()->add_item(Item(args[0]));
            }

            QS_TO_INT(y, args[1]);
            QS_TO_INT(x, args[2]);

            OOB_CHECK(y, x);

            gw()->game()->inventory().put_item(Item(args[0]), y, x);
        }
    },
    {
        "save",
        "Save the current state of the game to disk",
        0,
        [](const QStringList &) {
            gw()->save();
        }
    },
    {
        "load",
        "Load a saved character state to disk",
        0,
        [](const QStringList &) {
            gw()->load();
        }
    },
    {
        "name",
        "Change the current character's name to $0; if $0 is '?', show the current name instead",
        1,
        [](const QStringList &args) {
            QString new_name = args[0];
            if (new_name == "?") {
                qDebug("My name is (%s)", gw()->selected_char().name().toStdString().c_str());
                return;
            }

            gw()->selected_char().name() = new_name;
        }
    },
    {
        "energy",
        "Change the current character's energy to $0; if $0 is '?', show the current energy instead",
        1,
        [](const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current energy is (%lld)", gw()->selected_char().energy().amount());
                return;
            }

            QS_TO_INT(energy, args[0]);

            gw()->selected_char().energy().set(energy, &gw()->selected_char());
        }
    },
    {
        "spirit",
        "Change the current character's spirit to $0; if $0 is '?', show the current spirit instead",
        1,
        [](const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Current spirit is (%lld)", gw()->selected_char().spirit().amount());
                return;
            }

            QS_TO_INT(spirit, args[0]);

            gw()->selected_char().spirit().set(spirit, &gw()->selected_char());
        }
    },
    {
        "timel",
        "Change the amount of ms left in the current activity to $0; if $0 is '?', show the amount of ms left instead",
        1,
        [](const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Amount of ms left in activity is (%lld)", gw()->selected_char().activity().ms_left);
                return;
            }

            QS_TO_INT(ms, args[0]);

            gw()->selected_char().activity().ms_left = ms;
        }
    },
    {
        "effect",
        "Set the effect at n = $0 to a new item with name $1",
        2,
        [](const QStringList &args) {
            QS_TO_INT(n, args[0]);

            gw()->selected_char().effects()[n] = Item(args[1]);
        }
    },
    {
        "skill",
        "Set the skill at n = $0 to a new item with name $1",
        2,
        [](const QStringList &args) {
            QS_TO_INT(n, args[0]);

            gw()->selected_char().skills()[n] = Item(args[1]);
        }
    },
    {
        "couple",
        "Add an egg from explorers with names $0 and $1 to your inventory that hatches after 1 action",
        2,
        [](const QStringList &args) {
            auto &characters = gw()->game()->characters();

            CharacterId parent_id1 = std::find_if(begin(characters), end(characters), [&](Character &a) {
                return a.name() == args[0];
            })->id();

            CharacterId parent_id2 = std::find_if(begin(characters), end(characters), [&](Character &a) {
                return a.name() == args[1];
            })->id();

            Item egg = Item::make_egg(parent_id1, parent_id2);
            egg.instance_properties.map[InstanceEggFoundThreatstamp] = 0;
            gw()->game()->add_item(egg);
        }
    },
    {
        "egg",
        "Put a random found egg into the inventory",
        0,
        [](const QStringList &) {
            gw()->game()->add_item(Item::make_egg());
        }
    },
    {
        "baby",
        "Create a random explorer",
        0,
        [](const QStringList &) {
            gw()->game()->add_character(Generators::yokin_name(), { Generators::color() });
        }
    },
    {
        "fast",
        "If $0 is 0, turn off accelerated actions; otherwise turn it on",
        1,
        [](const QStringList &args) {
            QS_TO_INT(fast, args[0]);

            gw()->game()->fast_actions() = fast;
        }
    },
    {
        "threat",
        "Change the game's threat to $0; if $0 is '?', show the current threat instead",
        1,
        [](const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Threat counter at (%lld)", gw()->game()->threat());
                return;
            }

            QS_TO_INT(threat, args[0]);

            gw()->game()->threat() = threat;
        }
    },
    {
        "noexhaust",
        "If $0 is 0, turn off no exhaustion; otherwise turn it on",
        1,
        [](const QStringList &args) {
            QS_TO_INT(noexhaust, args[0]);

            gw()->game()->no_exhaustion() = noexhaust;
        }
    },
    {
        "loadup",
        "Fill the inventory with useful items.",
        0,
        [](const QStringList &) {
            gw()->game()->inventory().add_item(Item("bleeding_wildheart"));
            gw()->game()->inventory().add_item(Item("bleeding_wildheart"));
            gw()->game()->inventory().add_item(Item("bleeding_wildheart"));
            gw()->game()->inventory().add_item(Item("bleeding_wildheart"));
            gw()->game()->inventory().add_item(Item("bleeding_wildheart"));
            gw()->game()->inventory().add_item(Item("globfruit"));
            gw()->game()->inventory().add_item(Item("globfruit"));
            gw()->game()->inventory().add_item(Item("globfruit"));
            gw()->game()->inventory().add_item(Item("fireclay"));
            gw()->game()->inventory().add_item(Item("fireclay"));
            gw()->game()->inventory().add_item(Item("fireclay"));
            gw()->game()->inventory().add_item(Item("fireclay"));
            gw()->game()->inventory().add_item(Item("fireclay"));
            gw()->game()->inventory().add_item(Item("oolite"));
            gw()->game()->inventory().add_item(Item("oolite"));
            gw()->game()->inventory().add_item(Item("oolite"));
            gw()->game()->inventory().add_item(Item("scandiskium"));
            gw()->game()->inventory().add_item(Item("scandiskium"));
            gw()->game()->inventory().add_item(Item("scandiskium"));
            gw()->game()->inventory().add_item(Item("scandiskium"));
            gw()->game()->inventory().add_item(Item("scandiskium"));
            gw()->game()->inventory().add_item(Item("cobolt_bar"));
            gw()->game()->inventory().add_item(Item("cobolt_bar"));
            gw()->game()->inventory().add_item(Item("cobolt_bar"));
            gw()->game()->inventory().add_item(Item("cobolt_bar"));
            gw()->game()->inventory().add_item(Item("cobolt_bar"));
            gw()->game()->inventory().add_item(Item("seaquake"));
            gw()->game()->inventory().add_item(Item("hashcracker"));
            gw()->game()->inventory().add_item(Item("metamorphic_destructor"));
            gw()->game()->inventory().add_item(Item("scalped_remains"));
            gw()->game()->inventory().add_item(Item("scalped_remains"));
            gw()->game()->inventory().add_item(Item("recovered_journal"));
            gw()->game()->inventory().add_item(Item("recovered_journal"));
        }
    },
    {
        "nextrev",
        "Reveal the next $0 tiles on the map.",
        1,
        [](const QStringList &args) {
            QS_TO_INT(tiles, args[0]);

            gw()->game()->map().reveal_progress() += tiles;
            gw()->refresh_map();
        }
    },
    {
        "move",
        "Move the expedition to the location with name $0.",
        1,
        [](const QStringList &args) {
            gw()->game()->current_location_id() = LocationDefinition::get_def(args[0]).id;
        }
    },
    {
        "waste",
        "Print the number of waste actions taken at the current location.",
        0,
        [](const QStringList &) {
            qDebug("waste: %lld", gw()->game()->waste_action_counts()[gw()->game()->current_location_id()]);
        }
    },
    {
        "sigwhen",
        "Print the number of waste actions needed to find the $0th signature item at the current location.",
        1,
        [](const QStringList &args) {
            QS_TO_INT(n, args[0]);

            qDebug("sigwhen: %lld", gw()->game()->signature_requirements()[gw()->game()->current_location_id()][n]);
        }
    },
    {
        "lore",
        "Change the game's lore to $0; if $0 is '?' show the current lore instead",
        1,
        [](const QStringList &args) {
            if (args[0] == "?") {
                qDebug("Lore at (%lld)", gw()->game()->lore());
                return;
            }

            QS_TO_INT(lore, args[0]);

            gw()->game()->lore() = lore;
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
