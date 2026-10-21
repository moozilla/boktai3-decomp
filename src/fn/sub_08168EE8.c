// MATCHING_CONCESSION: register-pinning
#include "global.h"

s32 PauseMenu_GetEquipmentRowY(void *, s32, s32, s32, s32);

#define Hud_SlideTick(counter, selection, y)                                                       \
    do {                                                                                           \
        if (*(u8 *)(hud + (counter)) != 0) {                                                       \
            (*(u8 *)(hud + (counter)))--;                                                          \
            i = 0;                                                                                 \
            dst = hud + (y);                                                                       \
            do {                                                                                   \
                *(u16 *)dst = PauseMenu_GetEquipmentRowY(hud, i, *(s8 *)(hud + (selection)), 3,    \
                                                         *(u8 *)(hud + (counter)));                \
                dst += 0x60;                                                                       \
                i++;                                                                               \
            } while (i <= 2);                                                                      \
        }                                                                                          \
    } while (0)

void SceneHud_TickWidgetSlides(u8 *hud)
{
    register u8 *dst asm("r6");
    s32 i;
    Hud_SlideTick(0xC71, 0xC72, 0xCAA);
    Hud_SlideTick(0x13F3, 0xF48, 0xE4A);
    Hud_SlideTick(0xF4C, 0xF4A, 0xF92);
}
