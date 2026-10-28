#ifndef GUARD_ACTOR_ATTACHMENT_STATE_H
#define GUARD_ACTOR_ATTACHMENT_STATE_H
#include "actor_attachment.h"
#include "contact_swarm.h"
#include "ram.h"
struct ActorAttachmentPartResource {
    u32 unknown0;
    u16 tileBase;
};
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
    struct ContactPosition position; /* pad at 2E is the observed projected Y. */
    u8 frame, flags31, flags, action;
    u16 unknown34; /* Signed vertical accumulator in full-sprite updates. */
    u8 status[3], phase[3];
    u8 partMask;
    s8 slotId;
    u8 kind, resourceKind;
    s16 timer, thresholds[3];
    u8 active48, unknown49[3];
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
    const struct ActorAttachmentPartResource *partResource;
    struct ActorAttachmentSlotView slots[8];
    u32 occupiedMask;
};
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
void ActorAttachment_BeginPartJitter(struct ActorAttachmentSlotView *, s32);
void ActorAttachment_NotifySourceState(struct ActorAttachmentSlotView *);
#endif
