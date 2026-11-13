#ifndef GUARD_ACTOR_DECISION_H

#define GUARD_ACTOR_DECISION_H

#include "actor_state_commands.h"

s32 SceneVolume_ConsumePendingEvent(struct ActorState *);

s32 Mod(s32, s32);

void Path_GetPointPositionById(void *, int, int);

void AnimatedHazard_CreateProximity(void *, int, int, int);

void ActorMovement_FindQuarterTurnPoint(struct ActorState *,
                                        struct ContactPosition *);

void ActorExtension_ResetState(struct ActorState *);

#endif
