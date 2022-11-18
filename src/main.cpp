#include "main.h"
#include "generators.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    LKGameWindow game_window;
    game_window.show();

    CheatConsole console(&game_window);

    game_window.mutate_state([](State &s) {
        s.inventory[0] = Item("globfruit");
    });

    return app.exec();
}
