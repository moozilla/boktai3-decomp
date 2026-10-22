#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);
void sub_0815AAD4(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    if (actor != NULL)
        *(u16 *)(actor + 0x52e) = 1;
}