#include "main.h"
#include "generators.h"
#include "die.h"

int main(int argc, char **argv) {
#ifdef Q_NO_DEBUG
    std::set_terminate([]() {
        bugcheck(UncaughtUnknownException);
    });
#endif

    QApplication app(argc, argv);

#ifdef _WIN32
    app.setStyleSheet(
        "*[slot=\"true\"] { border: 1px solid #bbb; border-radius: 3px; background-color: white; }"
        "*[tooltip=\"true\"] { border-radius: 3px; }"
    );
#endif

    check_most_recent_build(argv[0]);

#ifdef Q_NO_DEBUG
    try {
#endif
        LKGameWindow::instantiate_singleton();

        if (!gw()->save_file_exists()) {
            new_game_prompt();
        } else {
            gw()->load();
        }

        gw()->show();
        gw()->refresh_ui();

        CheatConsole console(gw());
        if (argc == 2 && strcmp(argv[1], "cheat") == 0) {
            console.show();
        }

        return app.exec();
#ifdef Q_NO_DEBUG
    }
    catch (std::out_of_range &e) {
        bugcheck(OutOfRangeException, e.what());
    } catch (IO::RetryException &e) {
        bugcheck(DeserializationFailure);
    }
#endif
}

void new_game_prompt() {
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
        exit(1);
    }
}

void check_most_recent_build(char *program_name) {
    QNetworkAccessManager *nam = new QNetworkAccessManager;
    QObject::connect(nam, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->rawHeader("AOR-Ignore-Update")[0] == '0') {
            QFile program = QFile(program_name);
            program.open(QIODevice::ReadOnly);

            QCryptographicHash hasher(QCryptographicHash::Sha256);
            hasher.addData(program.readAll());
            QByteArray hash = hasher.result();

#ifdef _WIN32
            QByteArray latest = QByteArray::fromHex(reply->rawHeader("AOR-Latest-Hash-Windows"));
#else
            QByteArray latest = QByteArray::fromHex(reply->rawHeader("AOR-Latest-Hash-Linux"));
#endif

            if (latest != hash) {
                QMessageBox update_notify;
                update_notify.setText(
                    "A new version of Aegis of Rhodon is available."
                );
                update_notify.setInformativeText(
                    "Visit <a href=\"https://doughbyte.com/aut/aor\">doughbyte</a> to get the update."
                );
                update_notify.setIcon(QMessageBox::Information);
                update_notify.exec();
            }
        }
    });

    nam->get(QNetworkRequest(QUrl("https://doughbyte.com/aut/aor/")));
}
