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
    u8 unknown0[9];
    u8 direction;
    u8 unknownA[0x34 - 0xA];
    struct ContactPosition offset;
};
struct ActorAttachmentSlotView *sub_08065C40(struct ActorAttachmentManagerView *);
struct ActorAttachmentManagerView *sub_08066C34(void);
#endif
