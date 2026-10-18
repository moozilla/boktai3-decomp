#include "global.h"
/* Presentation_PlaySelectionSound: Independent switch-based sound picker living in the byte tail after SelectionMenu_ConfigureLayers; matches the existing low-confidence proposal in sym */
#define Presentation_PlaySelectionSound sub_0801A0D0
#define Presentation_PlaySelectionSound sub_0801A0D0
void Sound_PlaySong(int);
void Presentation_PlaySelectionSound(void *p, int selection)
{
    switch (selection) {
        case 0:
            Sound_PlaySong(0x394);
            break;
        case 1:
            Sound_PlaySong(0x395);
            break;
        case 2:
            Sound_PlaySong(0x396);
            break;
    }
}
