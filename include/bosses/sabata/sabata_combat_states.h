#ifndef GUARD_SABATA_COMBAT_STATES_H

#define GUARD_SABATA_COMBAT_STATES_H

#include "bosses/sabata/sabata_state_commands.h"

s32 Sabata_TakeReachedTarget(struct SabataActorState *);

s32 Mod(s32, s32);

void Path_GetPointPositionById(void *, int, int);

void AnimatedHazard_CreateProximity(void *, int, int, int);

void ActorMovement_FindQuarterTurnPoint(struct SabataActorState *,
                                        struct ContactPosition *);

void ActorExtension_ResetState(struct SabataActorState *);

#endif
