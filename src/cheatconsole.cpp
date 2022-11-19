#include "cheatconsole.h"

CheatConsole::CheatConsole(LKGameWindow *game)
    : game(game), console()
{
    console.setupUi(this);
    QObject::connect(console.submit_button, SIGNAL(clicked()), this, SLOT(consume_input()));

    show();
}

void CheatConsole::execute(const std::string &input) {
    qDebug("$ %s", input.c_str());

    QString input_string = QString::fromStdString(input);
    QStringList tokens = input_string.split(" ");

    std::string command_name = tokens.takeFirst().toStdString();

    auto match_name = [&command_name](CheatCommand command) -> bool { return command_name == command.name; };
    auto command = std::find_if(begin(COMMANDS), end(COMMANDS), match_name);

    if (command == end(COMMANDS)) {
        qWarning("Failed to find command (%s)", command_name.c_str());
        return;
    }

    if (tokens.size() != command->nargs) {
        qWarning("Wrong number of arguments (%d) for command (%s) (wanted %d)", tokens.size(), command->name.c_str(), command->nargs);
        return;
    }

    command->action(game, tokens);
}

void CheatConsole::consume_input() {
    execute(console.input->text().toStdString());
    console.input->clear();
}
