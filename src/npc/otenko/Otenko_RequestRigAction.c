#include "npc/otenko/otenko_actor.h"
#include "global.h"
#include "engine/ram.h"
void Otenko_RequestRigAction(void)
{
    struct OtenkoActor *npc = gSceneNpc;
    if (npc) {
        struct OtenkoEffectState *effect = &npc->effect;
        effect->projected = 1;
    }
}
