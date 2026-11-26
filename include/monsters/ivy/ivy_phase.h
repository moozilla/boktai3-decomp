#ifndef GUARD_IVY_PHASE_H
#define GUARD_IVY_PHASE_H
#include "scene/actor/scene_actor_control_phase.h"

#include "scene/motion/scene_actor_terrain_reaction.h"
/* Clear the phase and queue the vine's entry callback. */
static inline void Ivy_ResetPhase(u8 *p)
{
    SceneActorTerrain_EnterState((void *)p, 0, 0, 0);
}
#endif
