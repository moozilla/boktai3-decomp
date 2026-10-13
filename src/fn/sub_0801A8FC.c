#include "global.h"
void Sound_PlaySong(u32);
void sub_08019F18(u8 *, void (*)(void));
void SelectionMenu_AnimateChoice(void);
void sub_0801A8FC(u8 *p)
{
    if (p[0x1e] <= 1) {
        Sound_PlaySong(0xdd);
        sub_08019F18(p, SelectionMenu_AnimateChoice);
    }
}
