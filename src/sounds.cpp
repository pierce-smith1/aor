#include "sounds.h"

std::map<ItemDomain, QSoundEffect *> Sounds::activity_sounds() {
    static std::map<ItemDomain, QSoundEffect *> sounds = {
        { Eating, get_sound("eat.wav") },
        { MiningTool, get_sound("mine.wav") },
        { ForagingTool, get_sound("forage.wav") },
        { SmithingTool, get_sound("smith.wav") },
        { Coupling, get_sound("couple.wav") },
        { Defiling, get_sound("defile.wav") },
        { Trading, get_sound("trade.wav") },
        { Travelling, get_sound("trade.wav") },
    };

    return sounds;
}

QSoundEffect *Sounds::grab_sound() {
    static QSoundEffect *sound = get_sound("drag.wav");
    return sound;
}

QSoundEffect *Sounds::drop_sound() {
    static QSoundEffect *sound = get_sound("drop.wav");
    return sound;
}

QSoundEffect *Sounds::hover_sound() {
    static QSoundEffect *sound = get_sound("mouse.wav");
    return sound;
}

QSoundEffect *Sounds::get_sound(const QString &name) {
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl(QString("qrc:///assets/wav/%1").arg(name)));
    sound->setVolume(0.2);

    return sound;
}
