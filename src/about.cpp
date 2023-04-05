#include "about.h"
#include "main.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTextEdit>

AboutBox::AboutBox() {
    setLayout(new QVBoxLayout());

    AboutSlot *slot = new AboutSlot;
    layout()->addWidget(slot);
    layout()->setAlignment(slot, Qt::AlignCenter);

    QTextEdit *text = new QTextEdit(this);
    text->setText(
        "<b>Aegis of Rhodon</b>" + QString(" b%1.%2.%3<br>").arg(AOR_MAJOR_VERSION).arg(AOR_MINOR_VERSION).arg(AOR_PATCH_VERSION) +
        "<br>"
        "<b>Code and art</b> by Pierce \"Breadcrumbs\" Smith (pierce@doughbyte.com)<br>"
        "<b>Sound design</b> by prrrki<br>"
        "<br>"
        "<b>Thank you for playing, and thank you to everyone who has given advice, feedback and encouragement!</b><br>"
        "<br>"
        "Sounds used:<br>"
        "<b>Fruit crack by Clearwavsound</b> (https://freesound.org/people/Clearwavsound/sounds/524610/) (https://creativecommons.org/licenses/by/3.0/)<br>"
        "<b>Eating Apple by devilqube</b> (https://freesound.org/people/devilqube/sounds/370368/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Whoosh.wav by kingsrow</b> (https://freesound.org/people/kingsrow/sounds/181577/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>anvil - sound effect by JLopez500</b> (https://freesound.org/people/JLopez5000/sounds/567466/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>fire.crackling.mp3 by dobroide</b> (https://freesound.org/people/dobroide/sounds/4211/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "<b>rock lands on metal.aiff by SoundCollectah</b> (https://freesound.org/people/SoundCollectah/sounds/109358/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Gathering Stone Resources.flac by qubodup</b> (https://freesound.org/people/qubodup/sounds/184383/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Shoveling by andersmmg</b> (https://freesound.org/people/andersmmg/sounds/516318/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "<b>bushmovement.wav by j1987</b> (https://freesound.org/people/j1987/sounds/106114/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Wood Chop 9.wav by wavecal22</b> (https://freesound.org/people/wavecal22/sounds/537434/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Footsteps_6_Dirt_shoe.wav by monte32</b> (https://freesound.org/people/monte32/sounds/353799/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>FX_Coins.wav by Haykrich</b> (https://freesound.org/people/Haykrich/sounds/401539/) (https://creativecommons.org/licenses/by-nc/4.0/)<br>"
        "<b>lightsaber5.mp3 by gyzhor</b> (https://freesound.org/people/gyzhor/sounds/47127/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>RockFall1b.wav by AlanCat</b> (https://freesound.org/people/AlanCat/sounds/381645/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Fairy_sound.wav by AdriMB86</b> (https://freesound.org/people/AdriMB86/sounds/410259/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Stabbing and tearing flesh 2 by studiobonk</b> (https://freesound.org/people/studiobonk/sounds/649121/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "<b>Jacket/Sloth Rustle 8 by brandondelehoy</b> (https://freesound.org/people/brandondelehoy/sounds/494792/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>Rustling Foliage in Forest by sillygrizzlies</b> (https://freesound.org/people/sillygrizzlies/sounds/635053/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<b>MECH-KEYBOARD-01.wav by newagesoup</b> (https://freesound.org/people/newagesoup/sounds/348239/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "<br>"
        "AOR is built with Qt5 and is licensed under the LGPL (https://www.gnu.org/licenses/lgpl-3.0.en.html)<br>"
    );
    text->setReadOnly(true);
    layout()->addWidget(text);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    layout()->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    slot->refresh();
}
