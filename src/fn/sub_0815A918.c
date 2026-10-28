#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Player_EmitQueuedEffects(u8 *);

void sub_0815A918(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL)
        Player_EmitQueuedEffects(actor);
}
