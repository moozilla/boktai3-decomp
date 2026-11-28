#include "npc/story_character/state_actor.h"

extern void (*const gUnk_086125F0[])(void *ctx, struct StateActor *actor);

void Kuro_DispatchMode(void *ctx, struct StateActor *actor)
{
    gUnk_086125F0[actor->state](ctx, actor);
}
