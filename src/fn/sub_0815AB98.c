#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);

void Player_EnterKindFour(u8 *);
void sub_0815AB98(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    if (actor != NULL)
        Player_EnterKindFour(actor);
}
