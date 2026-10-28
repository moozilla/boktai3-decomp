#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);

void Player_EnterKindFour(u8 *);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Player_UpdateFacingSide(u8 *);
void ActorSequence_Begin(u8 *, s32, s32);

void sub_0815ABB8(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    u8 *player = actor;
    if (actor != NULL) {
        Player_EnterKindFour(actor);
        if (Script_SeekToKeyword('d'))
            actor[0x3a4] = Script_GetValue();
        Player_UpdateFacingSide(player);
        ActorSequence_Begin(player, 0x24, 0);
    }
}
