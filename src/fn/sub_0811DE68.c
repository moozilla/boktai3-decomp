#include "global.h"
#include "engine.h"
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, s32, s32);
s32 sub_0811DD94(s32 kind, s32 bit);
/* Script: stores 1 in the result variable if bit -i of flag set -k
   (sub_0811DD94, six sets in the game state) is set, else 0. */
void GameFlag_TestFromScript(void)
{
    u32 result[2];
    s32 kind, bit;
    sub_0821AAD8(result);
    kind = Script_GetKeywordValue('k', -1);
    bit = Script_GetKeywordValue('i', -1);
    if (kind < 0 || bit < 0)
        sub_0821B4C8(result, 0, 0);
    else if (sub_0811DD94(kind, bit))
        sub_0821B4C8(result, 0, 1);
    else
        sub_0821B4C8(result, 0, 0);
}
