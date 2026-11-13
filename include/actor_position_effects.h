#ifndef GUARD_ACTOR_POSITION_EFFECTS_H
#define GUARD_ACTOR_POSITION_EFFECTS_H
#include "actor_position.h"
#include "contact_actor.h"
#include "monster_particle_random.h"
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
void SpriteDrawable_SelectTileFrame(void *, void *, s32);
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneVolume_BeginPhase(void *, int);
void SceneEventActor_UpdateJitter(void *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
u32 NavMesh_FindRoute(void *, void *, void *);
#endif
