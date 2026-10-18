#include "global.h"
#include "engine.h"
s32 Inventory_GetConsumableId(s32 slot);
void Inventory_SetConsumableEntry(s32 slot, s32 value);
/* Script: stores -a in the first empty (negative) entry of the first 16 of
   game state table160 (+0x160); returns 1, or 0 if there is no -a or no room. */
s32 GameState_AddListEntryFromScript(void)
{
    s32 value, i;
    if (!Script_SeekToKeyword('a'))
        return 0;
    value = Script_GetValue();
    for (i = 0; i <= 15; i++) {
        if (Inventory_GetConsumableId(i) < 0) {
            Inventory_SetConsumableEntry(i, value);
            return 1;
        }
    }
    return 0;
}
