#pragma once

#include <sstream>

#include <QString>
#include <QTextStream>
#include <QMessageBox>

enum FatalErrorType {
    UncaughtUnknownException,
    ItemCodeNonexistant,
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
    IncorrectHookPayload,
    CallForUnknownHookType,
    ItemSlotByNameLookupMiss,
    NonExhaustivePayloadSerialization,
    NonExhaustivePayloadDeserialization,
    UnimplementedSlotInstall,
    LocationDefinitionLookupMiss,
    CoordsByLocationIdLookupMiss,
};

QString bugcheck_error_string(FatalErrorType type);

inline QString format(QTextStream &s) {
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
        "An internal assertion failed. <b>This is a bug; please report it.</b><br>"
        "Your save file may be useful for debugging, so consider backing it up before you restart the game."
    );
    bugcheck.setInformativeText(QString(
        "<b>%1</b> (%2)"
    ).arg(bugcheck_error_string(error)).arg(format(args...)));
    bugcheck.exec();
    exit(error);
}
