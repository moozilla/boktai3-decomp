#ifndef GUARD_SCENE_PROJECTILE_CONFIG_H
#define GUARD_SCENE_PROJECTILE_CONFIG_H
#include "scene_projectile_pool.h"
struct SceneProjectile *Projectile_FindFreeSlot(struct SceneProjectilePool *, s32 *);
s32 Projectile_SpawnAtPosition(struct SceneProjectileParams *, struct OrbitEffectPosition *);
void Projectile_InitParameters(struct SceneProjectileParams *);
void Projectile_AdjustHeading(struct SceneProjectile *);
void Projectile_SetLaunchParameters(struct SceneProjectileParams *params, u16 movement,
                                    u16 lifetime, u8 direction, u32 speed);
void Projectile_SetTurnParameters(struct SceneProjectileParams *params, u16 duration, u32 turn,
                                  u8 speed);
void Projectile_SetHomingParameters(struct SceneProjectileParams *params, u16 duration,
                                    const struct ContactPosition *target, u8 speed);
void Projectile_SetCollisionParameters(struct SceneProjectileParams *params, u32 type, u32 flags,
                                       u32 power, u32 element, u32 duration);
void Projectile_SetCollisionExtents(struct SceneProjectileParams *params, u16 x, u16 y, u16 z);
void Projectile_SetCollisionOffset_081FD49C(struct SceneProjectileParams *params, u16 x, u16 y,
                                            u16 z);
void Projectile_SetSpriteParameters(struct SceneProjectileParams *params, u32 graphicsId, u32 frame,
                                    u32 palette);
void Projectile_SetBehaviorCallback(struct SceneProjectileParams *params, void *callback);
void Projectile_SetScriptProcedure(struct SceneProjectileParams *params, u32 script, u16 sequence);
void Projectile_SetBehaviorMode(struct SceneProjectileParams *params, u16 mode);
void Projectile_SetTrackingTarget(struct SceneProjectileParams *params,
                                  const struct ContactPosition *target);
#endif
