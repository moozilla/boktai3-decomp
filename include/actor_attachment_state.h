#ifndef GUARD_ACTOR_ATTACHMENT_STATE_H
#define GUARD_ACTOR_ATTACHMENT_STATE_H
#include "actor_attachment.h"
#include "contact_swarm.h"
struct ActorAttachmentPartSprite {
    u32 flags;
    u8 unknown4[4];
    u16 tile;
    u8 width, height;
    u8 unknownC[0xC];
    struct ContactPosition position;
    u8 unknown20[8];
};
struct ActorAttachmentSlotView {
    struct ContactPosition *anchor;
    struct ContactPosition jitter[3];
    struct ContactPosition offset;
    void *update;
    struct ContactPosition position;
    u8 frame, flags31, flags, action;
    u16 unknown34;
    u8 status[3], phase[3];
    u8 partMask;
    s8 slotId;
    u8 kind, resourceKind;
    s16 timer, thresholds[3];
    u8 active, unknown49[3];
    s16 *value;
    s32 counters[3];
    void *source;
    struct ContactSwarmSprite sprite;
    struct ActorAttachmentPartSprite parts[3];
    struct DisplaySequenceState sequences[3];
    void *followActor;
};
struct ActorAttachmentManagerView {
    u8 unknown0[0x58];
    const void *partResource;
    struct ActorAttachmentSlotView slots[8];
    u32 occupiedMask;
};
extern struct ActorAttachmentManagerView *gUnk_02000124;
static inline u32 ActorAttachment_ReadByteFlags(const u8 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 ActorAttachment_TestByteFlags(const u8 *flags, u32 mask)
{
    if (ActorAttachment_ReadByteFlags(flags, mask))
        return TRUE;
    return FALSE;
}
void sub_08064D40(struct ActorAttachmentSlotView *, s32);
void sub_080645D4(struct ActorAttachmentSlotView *);
void sub_08217EAC(struct ActorAttachmentPartSprite *);
#endif
