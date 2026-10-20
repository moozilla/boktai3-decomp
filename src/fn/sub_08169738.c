// MATCHING_CONCESSION: register-pinning
#include "global.h"

void Anim_Remove(void *);

void SceneHud_RemoveAnimations(u8 *hud)
{
    u8 *c = hud + 0xF70;
    u8 *b = hud + 0xE28;
    u8 *a = hud + 0xC88;
    register s32 i asm("r6");
    i = 2;
    do {
        Anim_Remove(a);
        Anim_Remove(b);
        Anim_Remove(c);
        c += 0x60;
        b += 0x60;
        a += 0x60;
        i--;
    } while (i >= 0);
    c = hud + 0x12D0;
    b = hud + 0x11B0;
    a = hud + 0x1090;
    i = 2;
    do {
        Anim_Remove(a);
        Anim_Remove(b);
        Anim_Remove(c);
        c += 0x60;
        b += 0x60;
        a += 0x60;
        i--;
    } while (i >= 0);
}
