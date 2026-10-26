#define gPlayerCueFlags gEngineFlags
#include "global.h"
#include "player.h"
#include "engine.h"
void LinkPlayer_ResetToIdleState(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_BeginMode27(struct Player *, s32, s32, s32);
s32 Player_BeginDirectionalCue(struct Player *p, s32 mode)
{
    s32 result;
    if (gEngineFlags & 0x800) {
        LinkPlayer_ResetToIdleState(p);
        ActorSequence_Begin(p, 0x21, 0);
        return 0;
    }
    if (p->tetherMode == 6) {
        result = 1;
        PlayerCue_BeginMode27(p, p->facing, 1, 0);
    } else {
        result = 0;
        PlayerCue_BeginMode27(p, p->facing, mode, 0);
    }
    return result;
}
