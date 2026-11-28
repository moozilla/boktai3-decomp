#include "global.h"
#include "engine/engine.h"
#include "system/game_state/game_state.h"
/* 32-byte records of the table at 08E6092C */
struct Record {
    u32 words[8];
};
extern const struct Record gUnk_08E6092C[];
void Inventory_ClearEquipmentEntry(s32 index);
void Inventory_TryAddEquipmentEntry(struct Record *record);
/* Script: clears game state +0x72C (two words), the 48 entries reset by
   Inventory_ClearEquipmentEntry, the eight slots at +0x68 and +0x50, then -n count, -w count
   record ids (each record of 08E6092C copied and passed to Inventory_TryAddEquipmentEntry) and
   -q eight slot values. */
void GameState_ResetEntriesFromScript(void)
{
    s32 i, count;
    struct Record record;
    for (i = 0; i < 2; i++)
        (&gGameState->flags72C)[i] = 0;
    for (i = 0; i < 48; i++)
        Inventory_ClearEquipmentEntry(i);
    for (i = 0; i < 8; i++)
        *(i + gGameState->inventory) = -1;
    gGameState->unk50 = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w')) {
            for (i = 0; i < count; i++) {
                record = gUnk_08E6092C[Script_GetValue()];
                Inventory_TryAddEquipmentEntry(&record);
            }
        }
        if (Script_SeekToKeyword('q'))
            for (i = 0; i < 8; i++)
                *(i + gGameState->inventory) = Script_GetValue();
    }
}
