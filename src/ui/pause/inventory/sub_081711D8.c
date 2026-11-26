#include "global.h"
s32 Inventory_GetConsumableId(s32);
void PauseInventory_AccumulateConsumableEffects(s32, u16 *);
void sub_081711D8(u8 *p, s32 item, s16 *out)
{
    s16 tmp[3];
    s32 i;
    s16 *t;
    u16 *dst;
    s32 zero = 0;
    t = tmp;
    dst = (u16 *)(p + 0x48b0);
    for (i = 2; i >= 0; i--) {
        *dst = zero;
        *t = zero;
        t++;
        dst++;
    }
    if (Inventory_GetConsumableId(item) != -1) {
        u16 *a = (u16 *)(p + 0x48b0);
        u16 *b;
        s16 *o;
        PauseInventory_AccumulateConsumableEffects(item, a);
        b = (u16 *)(*(u8 **)(p + 0xa40) + 0x332);
        o = out;
        for (i = 2; i >= 0; i--) {
            *o = *a - *b;
            b++;
            a++;
            o++;
        }
    } else {
        s32 zero = 0;
        s16 *o = out + 2;
        do {
            *o = zero;
            o--;
        } while ((s32)o >= (s32)out);
    }
}