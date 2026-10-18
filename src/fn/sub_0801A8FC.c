#include "global.h"
void Sound_PlaySong(u32);
void SelectionMenu_SetCallback(u8 *, void (*)(void));
void SelectionMenu_AnimateChoice(void);
void SelectionMenu_StartAnimateChoice(u8 *p)
{
    if (p[0x1e] <= 1) {
        Sound_PlaySong(0xdd);
        SelectionMenu_SetCallback(p, SelectionMenu_AnimateChoice);
    }
}
