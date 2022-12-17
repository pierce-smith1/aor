#include "main.h"
#include "generators.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    LKGameWindow::instantiate_singleton();

    if (!gw()->save_file_exists()) {
        QMessageBox new_game_message;
        new_game_message.setText(
            "You don't appear to have an existing game."
        );
        new_game_message.setInformativeText(
            "Will you step foot into a new world?<br>"
            "<br>"
            "<i>(If you do have an existing game, make sure the save.rho file is in the same directory as this executable.)</i>"
        );
        new_game_message.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        new_game_message.setDefaultButton(QMessageBox::Yes);
        new_game_message.setIcon(QMessageBox::Question);

        if (new_game_message.exec() != QMessageBox::Yes) {
            return 1;
        }
    } else {
        gw()->load();
    }

    gw()->show();
    gw()->refresh_ui();

    //CheatConsole console(gw());

    return app.exec();
}
