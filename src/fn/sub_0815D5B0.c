#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void Hud_RegisterTimer(u16 *);
void Player_UpdateFacingSide(u8 *);
void Player_EnterKindTwo(u8 *);
void ActorSequence_Begin(u8 *, s32, s32);
void sub_08155D58(u8 *);
void Player_UpdateTimedChargeAction(u8 *);

void sub_0815D5B0(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL) {
        u16 *timer;
        s32 value;
        if (Script_SeekToKeyword('t')) {
            value = Script_GetValue();
            timer = (u16 *)(actor + 0x528);
        } else {
            timer = (u16 *)(actor + 0x528);
            value = 600;
        }
        *timer = value;
        Hud_RegisterTimer((u16 *)(actor + 0x528));
        if (Script_SeekToKeyword('c')) {
            value = Script_GetValue();
            timer = (u16 *)(actor + 0x52c);
        } else {
            timer = (u16 *)(actor + 0x52c);
            value = 720;
        }
        *timer = value;
        *(u16 *)(actor + 0x52a) = 0;
        *(u32 *)(actor + 0x5a4) = ScriptActor_GetCueOptionFromScript();
        actor[0x3a4] = 4;
        Player_UpdateFacingSide(actor);
        Player_EnterKindTwo(actor);
        ActorSequence_Begin(actor, 7, 0);
        if (actor[0x418] == 6)
            *(u32 *)(actor + 0x58c) = (u32)sub_08155D58;
        else
            *(u32 *)(actor + 0x58c) = (u32)Player_UpdateTimedChargeAction;
    }
}
