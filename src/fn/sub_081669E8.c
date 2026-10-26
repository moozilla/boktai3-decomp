#include "global.h"
extern u8 *gGameState;
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
u32 PauseMenu_IsCategoryEntryEmpty(u8 *s, u32 i, u32 k)
{
    u32 r;
    s32 v;
    switch (k) {
        case 0:
            if (*(s16 *)(i * 2 + gGameState + 0x828) != -1)
                goto no;
        yes:
            r = 1;
            goto end;
        case 1:
            if (*(s16 *)(i * 2 + gGameState + 0x4b0) != 0)
                goto no;
            goto yes;
        case 2:
            if (Inventory_GetEquipmentId(i) != 0xff)
                goto no;
            goto yes;
        case 3:
            v = *(s16 *)(i * 2 + gGameState + 0x160);
            goto chk;
        case 4:
            v = Inventory_GetQuantityByStorage(*(u8 *)(s + 0x4860), i);
        chk:
            if (v < 0)
                goto yes;
        default:
        no:
            r = 0;
    }
end:
    return r;
}
