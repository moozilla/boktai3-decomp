#include "npc/otenko/scene_npc_transition.h"
#include "engine/task.h"

void Otenko_DestroyTransitionEffect(struct TransitionEffect *effect)
{
    Task_Kill(&effect->task);
}
