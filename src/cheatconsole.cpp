#include "cheatconsole.h"

CheatConsole::CheatConsole(LKGameWindow *game)
    : game(game), console()
{
    console.setupUi(this);
    QObject::connect(console.submit_button, SIGNAL(clicked()), this, SLOT(consume_input()));

    show();
}

void CheatConsole::execute(const QString &input) {
    qDebug("$ %s", input.toStdString().c_str());

    QStringList tokens = input.split(" ");
    QString command_name = tokens.takeFirst();

    auto match_name = [&](CheatCommand command) -> bool {
        return command_name == command.name;
    };
    auto command = std::find_if(begin(COMMANDS), end(COMMANDS), match_name);

    if (command == end(COMMANDS)) {
        qWarning("Failed to find command (%s)", command_name.toStdString().c_str());
        return;
    }

    if (tokens.size() != command->nargs) {
        qWarning(
            "Wrong number of arguments (%d) for command (%s) (wanted %d)",
            tokens.size(),
            command->name.toStdString().c_str(),
            command->nargs
        );
        return;
    }

    command->action(game, tokens);
    game->refresh_ui();
}

void CheatConsole::consume_input() {
    execute(console.input->text());
    console.input->clear();
}
