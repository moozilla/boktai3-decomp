#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);

u8 sub_0815AB6C(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    if (actor == NULL)
        return 0;
    return actor[0xbf5];
}
