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
struct ActorAttachmentSlotView *
ActorAttachment_AllocateSlotHeader(struct ActorAttachmentManagerView *);
struct ActorAttachmentManagerView *ActorAttachment_CreateManager(void);
struct ActorAttachmentSlotView *ActorAttachment_ReleaseResource(struct ActorAttachmentSlotView *,
                                                                u32, void *);
struct ActorAttachmentSlotView *
ActorAttachment_ReleasePartsResource(struct ActorAttachmentSlotView *, u32, void *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateSpriteSlot(u32, struct ContactPosition *, void *, s32, s32, u32, void *,
                                 struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateValueSpriteSlot(u32, struct ContactPosition *, void *, s32, s32, u32, s16 *,
                                      s32, u32, void *, struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateSinglePartSlot(u32, struct ContactPosition *, void *, s32, u32,
                                     struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateThreePartSlot(u8, struct ContactPosition *, void *, s32, u32, s16 *, s32,
                                    struct ActorAttachmentFollowView *, u32);
struct ActorAttachmentSlotView *
ActorAttachment_CreateFixedSpriteSlot(struct ContactPosition *, void *, s32, s32, u32, void *,
                                      struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateAlternateSpriteSlot(struct ContactPosition *, void *, s32, s32, u32, void *,
                                          struct ActorAttachmentFollowView *);
struct ActorAttachmentSlotView *
ActorAttachment_CreateMeterPartsSlot(u32, struct ContactPosition *, void *, s32, u32, s16 *, s32,
                                     u32, struct ActorAttachmentFollowView *, u32);
struct ActorAttachmentSlotView *
ActorAttachment_CreatePartsSlotWithCount(u8, struct ContactPosition *, void *, s32, u32, s16 *, s32,
                                         u32, struct ActorAttachmentFollowView *, u32);
s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *);
#endif
