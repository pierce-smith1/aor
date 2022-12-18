#pragma once

#include <sstream>

#include <QString>
#include <QMessageBox>

enum FatalErrorType {
    ItemCodeNonexistant,
    UncaughtUnknownException,
};

QString bugcheck_error_string(FatalErrorType type);

inline std::string format(std::stringstream &ss)
{
    return ss.str();
}

template<typename T, typename... Args> inline std::string format(std::stringstream &ss, T first, Args... args)
{
    ss << first;
    return format(ss, args...);
}

template<typename T, typename... Args> inline std::string format(T first, Args... args)
{
    std::stringstream ss;
    return format(ss, first, args...);
}

template <typename... T> void bugcheck(FatalErrorType error, T... args) {
    QMessageBox bugcheck;
    bugcheck.setText(
        "A problem has been detected and Rhodon has been resealed to prevent damage<br>"
        "to the universe."
    );
    bugcheck.setInformativeText(
        bugcheck_error_string(error) + "<br>"
        "If this is the first time you've seen your consciousness ripped from Rhodon,<br>"
        "rest until your mind restarts. If this happens again, follow<br>"
        "these steps:<br>"
        "<br>"
        "Check to make sure any new organs have been properly installed.<br>"
        "If this is a new body, ask your diety for any updates you might need.<br>"
        "<br>"
        "If problems continue, disable or remove any newly installed organs.<br>"
        "Disable memory options such as facial recognition or object permanence.<br>"
        "If you need to use an anesthetic to remove or disable organs but do not have any,<br>"
        "suppress any memories of your friends and family until your soul falls alseep,<br>"
        "then proceed.<br>"
        "<br>"
        "Technical information:<br>"
        "*** STOP: " + format(args...)
    );
}
