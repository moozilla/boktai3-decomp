#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Player_EmitQueuedEffects(u8 *);

void sub_0815A918(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gUnk_02000580[i];
    if (actor != NULL)
        Player_EmitQueuedEffects(actor);
}
