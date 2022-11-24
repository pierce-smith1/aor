#include "main.h"
#include "generators.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    LKGameWindow game_window;
    game_window.show();

    game_window.refresh_ui();

    CheatConsole console(&game_window);

    return app.exec();
}
