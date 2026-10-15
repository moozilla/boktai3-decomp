#ifndef GUARD_ACTOR_ATTACHMENT_SLOT_H
#define GUARD_ACTOR_ATTACHMENT_SLOT_H
#include "contact_swarm.h"
/* Narrow shared slot prefix used from 6466C onward. Attachment identity is tentative. */
struct ActorAttachmentSource {
    u8 unknown0[0x98];
    u8 state;
    u8 unknown99[0x2B5 - 0x99];
    u8 category;
    u8 unknown2B6[0x2FA - 0x2B6];
    u16 ownerKey;
};
struct ActorAttachmentSlot {
    u8 unknown0[0x24];
    u32 unknown24;
    u8 unknown28[9];
    u8 flags31, flags32;
    u8 unknown33[3];
    u8 status[3], secondaryStatus[3];
    u8 unknown3C;
    s8 slotId;
    u8 unknown3E, kind;
    s16 timer, thresholds[3];
    u8 active, unknown49[3];
    u32 unknown4C;
    s32 counters[3];
    struct ActorAttachmentSource *source;
    struct ContactSwarmSprite sprite;
};
struct ActorAttachmentOwner {
    u8 unknown0[0xA1C];
    u32 occupiedSlots;
};
void ActorAttachment_NotifySourceState(struct ActorAttachmentSlot *);
#endif
