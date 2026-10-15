#ifndef GUARD_ACTOR_ATTACHMENT_MANAGER_H
#define GUARD_ACTOR_ATTACHMENT_MANAGER_H
/* Depends on the neighboring worker's shared slot layout. */
#include "actor_attachment_state.h"
struct ActorAttachmentManagerResources {
    u8 task[0x18];
    u8 graphicsA[0x1C], graphicsB[0x1C];
    void *sequenceA, *sequenceB;
    const void *partResource;
};
struct ActorAttachmentFollowView {
    void *identity;
    u32 flags;
    u8 unknown8;
    u8 direction;
    u8 unknownA[2];
    struct ContactPosition anchor;
    u8 unknown14[0x34 - 0x14];
    struct ContactPosition offset;
};
struct ActorAttachmentSlotView *sub_08065C40(struct ActorAttachmentManagerView *);
struct ActorAttachmentManagerView *sub_08066C34(void);
struct ActorAttachmentSlotView *sub_08065C50(struct ActorAttachmentSlotView *, u32, void *);
struct ActorAttachmentSlotView *sub_08065D1C(struct ActorAttachmentSlotView *, u32, void *);
struct ActorAttachmentSlotView *sub_08065DBC(u32, struct ContactPosition *, void *, s32, s32, u32,
                                             void *, struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *sub_08065FD0(u8, struct ContactPosition *, void *, s32, s32, u32,
                                             s16 *, s16, u32, void *,
                                             struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *sub_08066278(u32, struct ContactPosition *, void *, s32, u32,
                                             struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *sub_08066A88(u8, struct ContactPosition *, void *, s32, u32, s16 *,
                                             s32, struct ActorAttachmentFollowView *, u32);
struct ActorAttachmentSlotView *sub_080666D8(struct ContactPosition *, void *, s32, s32, u32,
                                             void *, struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *sub_080668B0(struct ContactPosition *, void *, s32, s32, u32,
                                             void *, struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *sub_08066474(u8, struct ContactPosition *, void *, s32, u32, s16 *,
                                             s32, u32, struct ActorAttachmentFollowView *, u32);
#endif
