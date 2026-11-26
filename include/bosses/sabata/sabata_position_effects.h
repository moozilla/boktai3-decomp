#ifndef GUARD_SABATA_POSITION_EFFECTS_H
#define GUARD_SABATA_POSITION_EFFECTS_H
#include "bosses/sabata/sabata_position.h"
#include "scene/collision/contact_actor.h"
#include "engine/math/monster_particle_random.h"
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
void SpriteDrawable_SelectTileFrame(void *, void *, s32);
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void Sabata_BeginCombatPhase(void *, int);
void Sabata_UpdateHitJitter(void *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
u32 NavMesh_FindRoute(void *, void *, void *);
#endif
