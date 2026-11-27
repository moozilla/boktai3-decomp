#include "global.h"
struct P {
    u8 f0[0xC72];
    s8 sel;
};
void PauseMenu_LoadAttributeIconFrame(u8 *, u8 *, s32);
s32 PauseMenu_GetEquipmentRowY(u8 *, s32, s32, s32, s32);
void PauseMenu_RefreshEquipmentSlotIcons(u8 *p)
{
    s32 i = 0;
    u8 *q = p + 0xC88;
    s8 *r = (s8 *)(p + 0xC78);
    for (; i <= 2; i++) {
        PauseMenu_LoadAttributeIconFrame(p, q, r[1]);
        *(u16 *)(q + 0x22) = PauseMenu_GetEquipmentRowY(p, i, ((struct P *)p)->sel, 3, 0);
        q += 0x60;
        r += 4;
    }
}
