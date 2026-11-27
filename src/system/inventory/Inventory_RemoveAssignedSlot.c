#include "global.h"

struct Inv0822DD80 {
    u8 pad0[0x50];
    s16 cursor;
    u8 pad52[0x68 - 0x52];
    u16 slots[8];
};
extern struct Inv0822DD80 *gGameState;
u32 Inventory_IsEquipmentAssigned(s32);

s32 Inventory_RemoveAssignedSlot(s32 slot)
{
    s32 list[8];
    s32 n = 0;
    s32 i;
    s32 *out;
    i = 0;
    out = list;
    for (; i <= 15; i++) {
        if (i == slot) {
            if (n <= 7) {
                s16 *c = &gGameState->cursor;
                s32 cur = *c;
                if (n == cur)
                    *c = 0;
                else if (n < cur)
                    (*(u16 *)c)--;
            }
        } else if (Inventory_IsEquipmentAssigned(i)) {
            *out++ = i;
            n++;
        }
    }
    if (n == 0)
        return 0;
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        *(u16 *)((u8 *)gGameState + i * 2 + 0x68) = list[i];
    return 1;
}
