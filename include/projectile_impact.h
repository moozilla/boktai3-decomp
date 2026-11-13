#ifndef GUARD_PROJECTILE_IMPACT_H
#define GUARD_PROJECTILE_IMPACT_H
#include "engine.h"
#include "task4e01_projectile.h"
s32 Projectile_UpdateAnimation(struct Task4E01Projectile *);
u32 Projectile_UpdateAnimationTail(struct Task4E01Projectile *);
s32 Projectile_InitResetDelay(struct Task4E01Projectile *);
s32 Projectile_UpdateResetDelay(struct Task4E01Projectile *);
s32 Projectile_SpawnImpactPair(struct Task4E01Projectile *);
void Projectile_InitFromType(struct Task4E01Params *);
void Task4E01_BeginImpactAnimation(struct Task4E01Projectile *);
#endif
