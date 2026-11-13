#include "actor_attachment_manager.h"
#include "gba/syscall.h"
#include "gba/macro.h"
void ParticleSprite_Initialize(void *, const void *, s32);
void Effect_SetOrigin(void *, s32, s32);
static inline const struct ActorAttachmentPartResource *
ActorAttachment_GetPartResource(struct ActorAttachmentManagerView *manager, s32 kind)
{
    switch (kind) {
        case 2:
            return manager->partResource;
        case 3:
            return manager->partResource;
        default:
            return NULL;
    }
}
static inline void ActorAttachment_SetupMeterPart(struct ActorAttachmentManagerView *manager,
                                                  struct ActorAttachmentSlotView *slot, s32 index)
{
    struct ActorAttachmentPartSprite *part;
    void *sequence;
    s32 xOffset, zOffset;
    zOffset = 0;
    xOffset = 0;
    part = &slot->parts[index];
    ParticleSprite_Initialize(part, ActorAttachment_GetPartResource(manager, slot->kind), 0);
    ActorAttachment_GetActionOffset(
        slot->resourceKind, slot->action,
        ((struct ActorAttachmentFollowView *)slot->followActor)->direction, &xOffset, &zOffset,
        slot->source);
    Effect_SetOrigin(part, xOffset, zOffset);
    if (slot->action == 255) {
        slot->offset = ((struct ActorAttachmentFollowView *)slot->followActor)->offset;
    } else {
        struct ContactPosition *offset = &slot->offset;
        CpuFill32(0, offset, sizeof(*offset));
    }
    part->flags |= 1;
    part->unknownC[4] = 127;
    part->unknownC[3] = 1;
    slot->sequences[index].unknownA = 255;
    switch (slot->kind) {
        case 0:
            sequence = ((struct ActorAttachmentManagerResources *)manager)->sequenceA;
            break;
        case 2:
        case 3:
            sequence = ((struct ActorAttachmentManagerResources *)manager)->sequenceB;
            break;
        case 1:
        case 4:
        case 5:
        default:
            sequence = NULL;
            break;
    }
    Anim_SetSequence(&slot->sequences[index], sequence, slot->frame, 0, 0);
}
struct ActorAttachmentSlotView *
ActorAttachment_CreateMeterPartsSlot(u32 frame, struct ContactPosition *anchor, void *update, s32 z,
                                     u32 action, s16 *value, s32 total, u32 partCount,
                                     struct ActorAttachmentFollowView *follow, u32 sourceWord)
{
    u8 startFrame = frame;
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentSlotView *slot;
    struct ActorAttachmentPartSprite *parts;
    s32 step;
    s32 i;
    s32 partIndex;
    existing = gActorAttachmentManager;
    if (!existing) {
        manager = ActorAttachment_CreateManager();
        if (!manager)
            return NULL;
    } else
        manager = existing;
    if (!follow)
        return NULL;
    step = Div(total, 3);
    slot = ActorAttachment_AllocateSlotHeader(manager);
    parts = slot->parts;
    if (!slot)
        return NULL;
    slot->frame = startFrame;
    slot->action = action;
    slot->value = value;
    slot->flags31 = 1;
    slot->kind = 3;
    slot->unknown34 = 0;
    slot->update = update;
    slot->position.y = 0;
    slot->position.z = z;
    for (i = 0; i <= 2; i++) {
        slot->status[i] = 0;
        slot->phase[i] = 0;
        slot->counters[i] = 0;
        slot->thresholds[i] = 0;
    }
    slot->timer = 0;
    slot->jitter[0].z = 0;
    slot->jitter[0].y = 0;
    slot->jitter[0].x = 0;
    slot->resourceKind = 3;
    slot->anchor = anchor;
    slot->partMask = 0;
    slot->active48 = 0;
    slot->source = (void *)sourceWord;
    slot->followActor = follow;
    for (partIndex = 0; partIndex < (s32)partCount; partIndex++) {
        slot->counters[partIndex] = 17 * partIndex;
        slot->phase[partIndex] = 0;
        slot->thresholds[partIndex] = partIndex * step;
        ActorAttachment_SetupMeterPart(manager, slot, partIndex);
        parts[partIndex].position = *anchor;
        slot->partMask |= 1 << partIndex;
    }
    return slot;
}
