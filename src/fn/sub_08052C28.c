#include "stacked_actor.h"
extern u8 *gUnk_02000580;
void sub_0815B858(u8 *, u32, u32);
s32 StackedActor_IsTetherNearby(struct StackedActor *);
s32 TetherActor_BeginKindFourStateNine(void);
s32 Script_Call(u32, void *);
void StackedActor_BeginPlayerInteraction(struct StackedActor *o)
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
        if (gUnk_02000580[0x457] == 0) {
            u8 *pf;
            sub_0815B858(gUnk_02000580, 4, 0);
            pf = &o->tetherAccepted;
            *pf = 0;
            if (StackedActor_IsTetherNearby(o)) {
                if (TetherActor_BeginKindFourStateNine())
                    *pf = 1;
            }
            o->sprite.flags &= ~0x200;
            o->sprite.renderMode = 3;
            o->shadow.flags |= 1;
            StackedActor_SetUpdate(o, StackedActor_ExtendForScript);
            if (o->pendingScriptId != 0)
                Script_Call(o->pendingScriptId, 0);
        }
    }
}
