#ifndef GUARD_BURST_EMITTER_H
#define GUARD_BURST_EMITTER_H
#include "contact_swarm.h"
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
extern struct BurstEmitterPool *gUnk_02000160;
extern u32 gUnk_030053F4;
extern void *gUnk_08605B78[];
void sub_08069964(struct BurstEmitter *);
void sub_08069B68(struct BurstEmitter *);
void sub_08069BC8(struct BurstEmitter *, s32, s32, s32, s32);
void sub_08069C98(struct BurstEmitterPool *, struct BurstEmitter *);
void sub_08069CD8(struct BurstEmitterPool *, struct BurstEmitter *);
struct BurstEmitter *sub_08069D0C(struct BurstEmitterPool *);
s32 sub_08069D50(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void sub_08069DB8(void);
s32 sub_08069ED0(struct BurstEmitterPool *);
void sub_08069F24(struct BurstEmitterPool *);
void sub_08069F30(void);
s32 sub_08069F34(struct BurstEmitterPool *, u32);
struct BurstEmitterPool *sub_08069F4C(void);
void sub_08069F98(void);
#endif
