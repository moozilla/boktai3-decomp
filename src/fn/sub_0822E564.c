#include "global.h"
#include "engine.h"
void GameState_AddEquipment(s32 value);
/* game state fields, addressed index-first as the original does */
#define STATE_U32(offset) (*(u32 *)((u8 *)gGameState + (offset)))
#define STATE_U16(offset) (*(u16 *)((u8 *)gGameState + (offset)))
#define SLOT(i)           (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x88))
/* Script: clears the unlock mask (+0x4C), the four slots at +0x88 and the
   selection (+0x52), then -n count, -m count values for GameState_AddEquipment and -q
   four slot values. */
void GameState_ResetSlotsFromScript(void)
{
    s32 i, count;
    STATE_U32(0x4C) = 0;
    for (i = 0; i <= 3; i++)
        SLOT(i) = -1;
    STATE_U16(0x52) = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('m'))
            for (i = 0; i < count; i++)
                GameState_AddEquipment(Script_GetValue());
        if (Script_SeekToKeyword('q'))
            for (i = 0; i <= 3; i++)
                SLOT(i) = Script_GetValue();
    }
}
