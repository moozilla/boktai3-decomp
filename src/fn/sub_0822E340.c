#include "global.h"
#include "engine.h"
#include "game_state.h"
void sub_0822E11C(s32 slot);
void sub_0822E0DC(s32 slot, s32 value);
/* game state party[i] (+0x58), addressed index-first as the original does */
#define PARTY(i) (*(s16 *)((u8 *)gUnk_02000710 + (i) * 2 + 0x58))
/* Script: clears game state flags720/724, the 48 entries of table160 and
   party[4], then refills them: -n count, -a count values for table160,
   -e four party ids. */
void GameState_ResetListsFromScript(void)
{
    s32 i, count;
    for (i = 0; i <= 1; i++)
        *(u32 *)((u8 *)gUnk_02000710 + i * 4 + 0x720) = 0;
    for (i = 0; i <= 47; i++)
        sub_0822E11C(i);
    for (i = 0; i <= 3; i++)
        PARTY(i) = -1;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('a'))
            for (i = 0; i < count; i++)
                sub_0822E0DC(i, Script_GetValue());
        if (Script_SeekToKeyword('e'))
            for (i = 0; i <= 3; i++)
                PARTY(i) = Script_GetValue();
    }
}
