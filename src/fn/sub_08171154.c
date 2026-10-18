#include "global.h"
s32 Inventory_GetConsumableId(u32);
u8 *GameState_GetConsumableDescriptor(s32);
void PauseInventory_AccumulateConsumableEffects(u32 a, u16 *t)
{
    u8 *e;
    s32 i;
    if (Inventory_GetConsumableId(a) != -1) {
        e = GameState_GetConsumableDescriptor(Inventory_GetConsumableId(a));
        i = 0;
        do {
            u32 k, v;
            if (i == 0) {
                k = e[1];
                v = e[2];
            } else {
                k = e[3];
                v = e[4];
            }
            switch (k) {
                case 1:
                    t[1] += v;
                    break;
                case 2:
                    t[2] += v;
                    break;
                case 3:
                    t[2] += v;
                case 4:
                case 5:
                case 6:
                    t[0] += v;
                    break;
            }
            i++;
        } while (i <= 1);
    }
}
