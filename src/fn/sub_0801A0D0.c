#include "global.h"
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
