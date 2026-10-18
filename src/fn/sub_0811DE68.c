#include "global.h"
#include "engine.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, s32, s32);
s32 sub_0811DD94(s32 kind, s32 bit);
/* Script: stores 1 in the result variable if bit -i of flag set -k
   (sub_0811DD94, six sets in the game state) is set, else 0. */
void GameFlag_TestFromScript(void)
{
    u32 result[2];
    s32 kind, bit;
    Script_ReadReference(result);
    kind = Script_GetKeywordValue('k', -1);
    bit = Script_GetKeywordValue('i', -1);
    if (kind < 0 || bit < 0)
        Script_WriteReferenceValue(result, 0, 0);
    else if (sub_0811DD94(kind, bit))
        Script_WriteReferenceValue(result, 0, 1);
    else
        Script_WriteReferenceValue(result, 0, 0);
}
