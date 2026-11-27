#include "objects/quest_barriers/quest_barrier.h"
u32 QuestBarrier_Tick(struct BreakableProp *prop)
{
    if (prop->destroyRequested != 0) {
        u32 *scriptId;
        u32 script;
        prop->sprite.flags |= 1;
        QuestBarrier_EmitBreakParticles(prop);
        scriptId = &prop->scriptId;
        script = *scriptId;
        if (script != 0) {
            *scriptId = 0;
            Script_Call(script, 0);
        }
        Task_Kill(prop);
    }
    return 0;
}
