#include "objects/warp_circle/warp_circle.h"
#include "recovered/script_call_arguments_layout.h"
s32 Script_Call(u32, struct ScriptCallArguments *);
void WarpCircle_EmitScriptParticles(struct WarpCircle *actor)
{
    u32 next_2;
    u32 frame = actor->frame;
    u32 shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask) {
            WarpCircle_InitFallingParticle(&actor->particles[shifted >> 18], &actor->world);
        }
    }
    {
        u32 next = actor->frame + 1;
        actor->frame = next;
        next_2 = (u16)next;
        if (next_2 == 64) {
            if (actor->particleScriptId) {
                u32 value;
                struct ScriptCallArguments args;
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
