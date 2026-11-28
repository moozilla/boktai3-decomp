#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
s32 SolarFruit_GetInventorySlot(s32 index);
void SolarFruit_SetInventorySlot(s32 index, s32 value);
s32 SolarFruit_IsInventorySlotEquipped(s32 index);

void sub_0822EB14(s32 a, s32 b)
{
    s32 entry = SolarFruit_GetInventorySlot(a);
    s32 i, n, sel;
    s32 list[8];
    s32 *p;

    SolarFruit_SetInventorySlot(a, SolarFruit_GetInventorySlot(b));
    SolarFruit_SetInventorySlot(b, entry);
    for (i = 0; i <= 7; i++) {
        if ((&gGameState->slots88[4])[i] == a)
            (&gGameState->slots88[4])[i] = b;
        else if ((&gGameState->slots88[4])[i] == b)
            (&gGameState->slots88[4])[i] = a;
    }
    n = 0;
    for (i = 0, p = list; i <= 7; i++) {
        if (SolarFruit_IsInventorySlotEquipped(i)) {
            *p++ = i;
            n++;
        }
    }
    sel = -1;
    for (i = 0; i < n; i++) {
        if ((&gGameState->slots88[4])[gGameState->actionAuxiliarySlot] == list[i]) {
            sel = i;
            break;
        }
    }
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        (&gGameState->slots88[4])[i] = list[i];
    if ((u32)sel <= 7)
        gGameState->actionAuxiliarySlot = sel;
}
