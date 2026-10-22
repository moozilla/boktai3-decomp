#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);
s32 Player_IsActiveInLocomotionState(u8 *);

s32 sub_0815A554(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    if (actor == NULL)
        return 0;
    return Player_IsActiveInLocomotionState(actor);
}
