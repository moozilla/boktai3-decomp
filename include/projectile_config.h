#ifndef GUARD_PROJECTILE_CONFIG_H
#define GUARD_PROJECTILE_CONFIG_H
#include "timed_effect_manager.h"
void Projectile_InitFromType(struct Task4E01Params *);
s32 TimedEffect_SpawnProjectile(struct TimedEffectManager *, struct Task4E01Params *,
                                const struct ContactPosition *);
s32 Projectile_Spawn(struct Task4E01Params *, void *);
s32 Projectile_CreateForOwner(struct Task4E01Params *, void *, u32);
void Projectile_InitializeParams(struct Task4E01Params *);
void Projectile_SetFlightParams(struct Task4E01Params *, u16, u16, s32, u32);
void Projectile_SetVerticalMotion(struct Task4E01Params *, u16, u32, u8);
void Projectile_SetDamageParams(struct Task4E01Params *, u32, u32, u32, u32, u32);
void Projectile_SetUniformCollisionSize(struct Task4E01Params *, u32);
void Projectile_SetCollisionOffset(struct Task4E01Params *, s32, s32, s32);
void Projectile_SetCollisionMask(struct Task4E01Params *, u16);
void Projectile_SetSpriteParams(struct Task4E01Params *, u32, u32, s32);
void Projectile_SetTypeAndElement(struct Task4E01Params *, u32, u32);
void Projectile_SetVariant(struct Task4E01Params *, u16);
s32 Task4E01_RemoveEntry(s32);
s32 Projectile_SetEntryDirection(u8, s32);
struct ContactPosition *Task4E01_GetEntryPosition(s32);
s32 Task4E01_GetKind7Value(s32);
s32 SceneEntry_SetPosition(s32, struct ContactPosition *);
s32 Task4E01_Destroy(void);
void Task4E01_OnDefaultTimedAnimation(void);
#endif
