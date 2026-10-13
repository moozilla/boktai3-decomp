#include "global.h"
extern u16 gUnk_03005260[];
void Anim_LoadKeyframe(u8 *, u8 *, int, int);
void Sound_PlaySong(int);
void SelectionMenu_HandleHorizontalInput(u8 *p)
{
    if ((gUnk_03005260[1] & 0x20) && p[0x20] == 1) {
        p[0x20] = 0;
        Anim_LoadKeyframe(p + 0x254, p + 0x4c, 0x4b, 0);
        Sound_PlaySong(0xdc);
    }
    if ((gUnk_03005260[1] & 0x10) && p[0x20] == 0) {
        p[0x20] = 1;
        Anim_LoadKeyframe(p + 0x254, p + 0x4c, 0x4c, 0);
        Sound_PlaySong(0xdc);
    }
}
