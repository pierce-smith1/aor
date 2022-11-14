#include "main.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    LKGameWindow game_window;
    game_window.show();

    CheatConsole console(&game_window);

    game_window.character.inventory[0] = Item("globfruit");
    game_window.refresh_inventory();

    return app.exec();
}
