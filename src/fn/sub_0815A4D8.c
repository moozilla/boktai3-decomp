#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gGameState;

s32 sub_0815A4D8(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    u8 *p = actor;
    if (actor == NULL)
        return *(s16 *)(gGameState + 0x36);
    if ((u8)(actor[0x457] - 4) <= 1)
        return (p[0x4a5] + 4) & 7;
    return actor[0x3a4];
}
