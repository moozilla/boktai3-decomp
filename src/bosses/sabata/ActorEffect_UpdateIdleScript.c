#include "bosses/sabata/sabata_attack_state.h"
extern u32 gUnk_02000560;
struct EffectScriptArgs {
    u32 count;
    s32 *values;
};
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void ActorEffect_UpdateIdleScript(struct SabataAttackActor *actor)
{
    struct SabataAttackActor *scriptsActor = actor;
    /* This path fills the argument slots and passes the adjacent uninitialized descriptor. */
    struct {
        s32 values[8];
        struct EffectScriptArgs args;
    } call;
    if (Sabata_TakeQueued(actor)) {
        actor->alternateDirection = actor->requestedDirection;
        {
            u32 animation = 14;
            actor->state = animation;
        }
    } else if (actor->pending) {
        s32 busy = Player_IsSlotInActionTwelveOrThirtyEight(0);
        if (!busy) {
            u32 script = scriptsActor->scripts[scriptsActor->decisionMode];
            if (script) {
                u32 argument = actor->direction;
                s16 *position = actor->position;
                u32 mask = 4;
                if (!(gUnk_02000560 & mask)) {
                    call.values[0] = position[0];
                    call.values[1] = position[1];
                    call.values[2] = position[2];
                    call.values[3] = argument;
                    Script_Call(script, &call.args);
                }
                scriptsActor->scripts[scriptsActor->decisionMode] = busy;
            }
        }
    }
    actor->actionTimer++;
}
