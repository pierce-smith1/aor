#pragma once

#include <sstream>

#include <QString>
#include <QTextStream>
#include <QMessageBox>

enum FatalErrorType : int {
    ItemCodeNonexistant = 1,
    UncaughtUnknownException,
    ProductsForUnknownDomain,
    InjuriesForUnknownDomain,
    AssessmentForUnknownDomain,
    NonEffectIntoEffectSlot,
    EncyclopediaSlotSet,
    PortraitSlotGet,
    PortraitSlotSet,
    SaveInvalidHeader,
    EmptyListSample,
    EmptyIdRef,
    ItemByIdLookupMiss,
    DefLookupMiss,
    CodeLookupMiss,
    OutOfRangeException,
    DeserializationFailure,
    CharacterByIdLookupMiss,
    QueuedActivitySlotGet,
    QueuedActivitySlotSet,
    NoMoreActivitySlots,
    ActivityByIdLookupMiss,
    NoStringForActionDomain,
    NoDescriptionForActionDomain,
};

QString bugcheck_error_string(FatalErrorType type);

inline QString format(QTextStream &s)
{
    QString string = s.readAll();
    return string.left(string.size() - 2);
}

inline QString format() {
    return "";
}

template<typename T, typename... Args> QString format(QTextStream &s, T first, Args... args) {
    s << first << ", ";
    return format(s, args...);
}

template<typename T, typename... Args> QString format(T first, Args... args) {
    QString u;
    QTextStream s(&u);
    return format(s, first, args...);
}

template <typename... T> void bugcheck(FatalErrorType error, T... args) {
    QMessageBox bugcheck;
    bugcheck.setStyleSheet("font-family: monospace");
    bugcheck.setIcon(QMessageBox::Critical);
    bugcheck.setText(
        "A problem has been detected and Rhodon has been resealed to prevent damage<br>"
        "to the universe."
    );
    bugcheck.setInformativeText(QString(
        "<b>%1</b><br><br>"
        "If this is the first time you've seen your consciousness ripped from Rhodon,<br>"
        "please close your eyes and slowly count to 10. If this happens again, follow<br>"
        "these steps:<br>"
        "<br>"
        "Check to make sure any new organs have been properly installed.<br>"
        "If this is a new body, ask your deity for any updates you might need.<br>"
        "<br>"
        "If problems continue, disable or remove any newly installed organs.<br>"
        "Disable memory options such as facial recognition or object permanence.<br>"
        "If you need to use an anesthetic to remove or disable organs,<br>"
        "suppress any memories of your friends and family until your soul is quiet,<br>"
        "then proceed.<br>"
        "<br>"
        "Technical information:<br>"
        "*** STOP: %2"
    ).arg(bugcheck_error_string(error)).arg(format(args...)));
    bugcheck.exec();
    exit(error);
}
