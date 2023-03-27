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
        "<b>Aegis of Rhodon</b><br>"
        "<br>"
        "<b>Code, art, & design</b> by Pierce Smith (pierce@doughbyte.com)<br>"
        "<b>Sound design</b> by prrrki<br>"
        "<br>"
        "Sounds used:<br>"
        "Fruit crack by Clearwavsound (https://freesound.org/people/Clearwavsound/sounds/524610/) (https://creativecommons.org/licenses/by/3.0/)<br>"
        "Eating Apple by devilqube (https://freesound.org/people/devilqube/sounds/370368/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Whoosh.wav by kingsrow (https://freesound.org/people/kingsrow/sounds/181577/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "anvil - sound effect by JLopez500 (https://freesound.org/people/JLopez5000/sounds/567466/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "fire.crackling.mp3 by dobroide (https://freesound.org/people/dobroide/sounds/4211/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "rock lands on metal.aiff by SoundCollectah (https://freesound.org/people/SoundCollectah/sounds/109358/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Gathering Stone Resources.flac by qubodup (https://freesound.org/people/qubodup/sounds/184383/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Shoveling by andersmmg (https://freesound.org/people/andersmmg/sounds/516318/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "bushmovement.wav by j1987 (https://freesound.org/people/j1987/sounds/106114/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Wood Chop 9.wav by wavecal22 (https://freesound.org/people/wavecal22/sounds/537434/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Footsteps_6_Dirt_shoe.wav by monte32 (https://freesound.org/people/monte32/sounds/353799/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "FX_Coins.wav by Haykrich (https://freesound.org/people/Haykrich/sounds/401539/) (https://creativecommons.org/licenses/by-nc/4.0/)<br>"
        "lightsaber5.mp3 by gyzhor (https://freesound.org/people/gyzhor/sounds/47127/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "RockFall1b.wav by AlanCat (https://freesound.org/people/AlanCat/sounds/381645/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Fairy_sound.wav by AdriMB86 (https://freesound.org/people/AdriMB86/sounds/410259/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Stabbing and tearing flesh 2 by studiobonk (https://freesound.org/people/studiobonk/sounds/649121/) (https://creativecommons.org/licenses/by/4.0/)<br>"
        "Jacket/Sloth Rustle 8 by brandondelehoy (https://freesound.org/people/brandondelehoy/sounds/494792/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "Rustling Foliage in Forest by sillygrizzlies (https://freesound.org/people/sillygrizzlies/sounds/635053/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
        "MECH-KEYBOARD-01.wav by newagesoup (https://freesound.org/people/newagesoup/sounds/348239/) (https://creativecommons.org/publicdomain/zero/1.0/)<br>"
    );
    text->setReadOnly(true);
    layout()->addWidget(text);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    layout()->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    slot->refresh();
}
