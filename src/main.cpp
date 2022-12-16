#include "main.h"
#include "generators.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    LKGameWindow::instantiate_singleton();
    gw()->show();
    gw()->refresh_ui();

    //CheatConsole console(gw());

    return app.exec();
}
