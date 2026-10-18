#define MenuTransition_UpdateSelectionFade    sub_0816EB30
#define MenuTransition_SetCallback Hud_SetInteractionCallback
#define MenuTransition_FinishSelectionMessage sub_0816EBE4
#include "global.h"
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_InitSelectionBounds(u32, u32);
void sub_0816D634(void *, u32);
void sub_0816EBE4(void);
struct MenuFadeView {
    u8 unknown0[0xa4e];
    u16 timer;
};
void sub_0816EB30(u8 *p)
{
    if (((struct MenuFadeView *)p)->timer == 0) {
        sub_0816D634(p, p[0x438a]);
        *(u32 *)(p + 0x2aa8) |= 1;
        if (p[0x4861])
            Sound_PlaySong(0x27f);
        else
            Sound_PlaySong(0x27e);
    }
    if (p[0x4861])
        Hud_InitSelectionBounds(((struct MenuFadeView *)p)->timer + 16, 0);
    else
        Hud_InitSelectionBounds(((struct MenuFadeView *)p)->timer + 16, 0x7fff);
    if (++((struct MenuFadeView *)p)->timer > 48)
        Hud_SetInteractionCallback(p, sub_0816EBE4, 1);
}
