#include "global.h"
#include "engine.h"
#define GameState_AddListEntryFromScript sub_0822E304
s32 sub_0822E104(s32 slot);
void sub_0822E0DC(s32 slot, s32 value);
/* Script: stores -a in the first empty (negative) entry of the first 16 of
   game state table160 (+0x160); returns 1, or 0 if there is no -a or no room. */
s32 GameState_AddListEntryFromScript(void)
{
    s32 value, i;
    if (!Script_SeekToKeyword('a'))
        return 0;
    value = Script_GetValue();
    for (i = 0; i <= 15; i++) {
        if (sub_0822E104(i) < 0) {
            sub_0822E0DC(i, value);
            return 1;
        }
    }
    return 0;
}
