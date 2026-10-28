#ifndef GUARD_BURST_EMITTER_H
#define GUARD_BURST_EMITTER_H
#include "contact_swarm.h"
#include "engine_globals.h"
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
    u8 task[0x18];
    u32 used;
    struct BurstEmitter slots[5];
};
extern struct BurstEmitterPool *gBurstEmitterPool;
extern void *gUnk_08605B78[];
void BurstEmitter_EmitFrame(struct BurstEmitter *);
void BurstEmitter_PlayAmbientSound(struct BurstEmitter *);
void BurstEmitter_InitCollider(struct BurstEmitter *, s32, s32, s32, s32);
void BurstEmitter_TickActive(struct BurstEmitterPool *, struct BurstEmitter *);
void BurstEmitter_ReleaseSlot(struct BurstEmitterPool *, struct BurstEmitter *);
struct BurstEmitter *BurstEmitter_AllocateSlot(struct BurstEmitterPool *);
s32 BurstEmitter_Create(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void BurstEmitter_CreateFromScript(void);
s32 BurstEmitter_UpdatePool(struct BurstEmitterPool *);
void BurstEmitter_DestroyPool(struct BurstEmitterPool *);
void BurstEmitter_ResetPool(void);
s32 BurstEmitter_InitPool(struct BurstEmitterPool *, u32);
struct BurstEmitterPool *BurstEmitter_CreatePool(void);
void BurstEmitter_ClearPool(void);
#endif
