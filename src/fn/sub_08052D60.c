#include "stacked_actor.h"
struct ScriptParticleArgs {
    u32 count : 16;
    u32 pad : 16;
    u32 *values;
};
s32 Script_Call(u32, struct ScriptParticleArgs *);
void StackedActor_EmitScriptParticles(struct StackedActor *actor)
{
    u32 next_2;
    u32 frame = actor->frame;
    u32 shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask) {
            StackedActor_InitFallingParticle(&actor->particles[shifted >> 18], &actor->world);
        }
    }
    {
        u32 next = actor->frame + 1;
        actor->frame = next;
        next_2 = (u16)next;
        if (next_2 == 64) {
            if (actor->particleScriptId) {
                u32 value;
                struct ScriptParticleArgs args;
                args.count = 1;
                args.values = &value;
                value = actor->particleScriptValue;
                Script_Call(actor->particleScriptId, &args);
            }
            if (actor->tetherAccepted && actor->acceptedScriptId) {
                Script_Call(actor->acceptedScriptId, 0);
            }
        }
    }
}
