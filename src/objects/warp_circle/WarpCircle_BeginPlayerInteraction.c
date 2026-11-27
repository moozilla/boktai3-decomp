#include "objects/warp_circle/warp_circle.h"
#include "player/player.h"
void PlayerCue_Motion7Turn(u8 *, u32, u32);
s32 WarpCircle_IsTetherNearby(struct WarpCircle *);
s32 TetherActor_BeginKindFourStateNine(void);
s32 Script_Call(u32, void *);
void WarpCircle_BeginPlayerInteraction(struct WarpCircle *o)
{
    u8 *pb = &o->lift;
    u32 v = *pb;
    if (v > 1)
        v -= 2;
    else
        v = 0;
    *pb = v;
    o->sprite.position.y = o->world.y + o->lift;
    if (o->lift == 0) {
        if ((*(u8 **)gPlayers)[0x457] == 0) {
            u8 *pf;
            PlayerCue_Motion7Turn((*(u8 **)gPlayers), 4, 0);
            pf = &o->tetherAccepted;
            *pf = 0;
            if (WarpCircle_IsTetherNearby(o)) {
                if (TetherActor_BeginKindFourStateNine())
                    *pf = 1;
            }
            o->sprite.flags &= ~0x200;
            o->sprite.renderMode = 3;
            o->shadow.flags |= 1;
            WarpCircle_SetUpdate(o, WarpCircle_ExtendForScript);
            if (o->pendingScriptId != 0)
                Script_Call(o->pendingScriptId, 0);
        }
    }
}
