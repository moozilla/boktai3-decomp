#include "global.h"
#include "engine.h"
void sub_0822E650(s32 index, s32 value);
void sub_0822E668(s32 value);
void sub_0822E6A4(s32 value, s32 slot);
/* game state fields, addressed index-first as the original does */
#define STATE_U16(offset) (*(u16 *)((u8 *)gUnk_02000710 + (offset)))
#define SLOT78(i)         (*(s16 *)((u8 *)gUnk_02000710 + (i) * 2 + 0x78))
/* Script: clears the twelve entries set by sub_0822E650, the eight slots at
   +0x78 and +0x50, then -n count, -w count values (nonzero ones go to
   sub_0822E668) and -q eight values for sub_0822E6A4 (+0x50 = 8). */
void GameState_ResetUnlocksFromScript(void)
{
    s32 i, count;
    for (i = 0; i <= 11; i++)
        sub_0822E650(i, 0);
    for (i = 0; i <= 7; i++)
        SLOT78(i) = -1;
    STATE_U16(0x50) = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w')) {
            for (i = 0; i < count; i++) {
                s32 value = Script_GetValue();
                if (value)
                    sub_0822E668(value);
            }
        }
        if (Script_SeekToKeyword('q')) {
            STATE_U16(0x50) = 8;
            for (i = 0; i <= 7; i++)
                sub_0822E6A4(Script_GetValue(), i);
        }
    }
}
