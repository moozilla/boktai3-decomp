#include "global.h"
void Sound_PlaySong(u32);
void PauseMenu_SmoothPoint(u8 *, u8 *, u32);
void PauseMenu_AddSpriteFlags(u8 *, u32, u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void sub_08174270(void);
void Hud_UpdateInventoryDiscardAnimation(u8 *p)
{
    u16 *c = (u16 *)(p + 0xA4E);
    if (*c == 0)
        Sound_PlaySong(0x38A);
    *c = *c + 1;
    if (*c <= 0xb) {
        PauseMenu_SmoothPoint(p + 0x2AC0, p + 0x442C, 0xc - *c);
    } else {
        PauseMenu_AddSpriteFlags(p, 0x3c, 1);
        Hud_SetInteractionCallback(p, sub_08174270, 1);
    }
}
