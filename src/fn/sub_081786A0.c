#include "global.h"
#include "area_effect_link.h"
u16 *Hud_GetTilemapCell(int, int, int);
struct SlotOwner {
    u8 unknown0[0x53];
    u8 flag;
    u8 unknown54[0x164 - 0x54];
    u8 slots[5];
};
void SceneHud_DrawSlotMarkers(void)
{
    u8 *src;
    u16 *dst;
    int i;
    struct SlotOwner *owner = gLinkSession;
    if (owner) {
        u8 f = owner->flag;
        src = owner->slots;
        if (f == 0)
            goto done;
    }
    src = 0;
done:
    dst = Hud_GetTilemapCell(0, 25, 2);
    i = 4;
    do {
        u32 c = *src;
        if (c == 0)
            *dst = 0xE0C0;
        else if (c == 1)
            *dst = 0xE0C3;
        else if (c == 2)
            *dst = 0xE0C2;
        else if (c == 3)
            *dst = 0xE0C1;
        else
            *dst = 0xE0C4;
        dst++;
        src++;
    } while (--i >= 0);
}
