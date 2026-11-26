#ifndef GUARD_BURST_EMITTER_H
#define GUARD_BURST_EMITTER_H
#include "scene/collision/contact_swarm.h"
#include "engine/engine_globals.h"
#include "engine/task.h"
/* Tentative five-slot burst family; kind/ownership identities are unconfirmed. */
struct BurstEmitter {
    struct ContactCollider collider;
    struct ContactPosition position;
    u16 frame, unknown5E;
    u8 mode, state;
    s8 index;
    u8 unknown63;
    u32 context;
};
struct BurstEmitterPool {
    struct Task task;
    u32 used;
    struct BurstEmitter slots[5];
};
/* 0x02000160 */
extern struct BurstEmitterPool *gBurstEmitterPool;
extern void *gUnk_08605B78[];
void SparkExplosion_EmitFrame(struct BurstEmitter *);
void SparkExplosion_PlayAmbientSound(struct BurstEmitter *);
void SparkExplosion_InitCollider(struct BurstEmitter *, s32, s32, s32, s32);
void SparkExplosion_TickActive(struct BurstEmitterPool *, struct BurstEmitter *);
void SparkExplosion_ReleaseSlot(struct BurstEmitterPool *, struct BurstEmitter *);
struct BurstEmitter *SparkExplosion_AllocateSlot(struct BurstEmitterPool *);
s32 SparkExplosion_Create(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void SparkExplosion_CreateFromScript(void);
s32 SparkExplosion_UpdatePool(struct BurstEmitterPool *);
void SparkExplosion_DestroyPool(struct BurstEmitterPool *);
void SparkExplosion_ResetPool(void);
s32 SparkExplosion_InitPool(struct BurstEmitterPool *, u32);
struct BurstEmitterPool *SparkExplosion_CreatePool(void);
void SparkExplosion_ClearPool(void);
#endif
