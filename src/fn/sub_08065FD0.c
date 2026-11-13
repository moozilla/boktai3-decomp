#include "actor_attachment_manager.h"
static inline u8 ActorAttachment_IsAlternateMode(void)
{
    u32 bit = 0x80;
    if (gGameState->flags868 & bit)
        return TRUE;
    return FALSE;
}
static inline void *ActorAttachment_GetSpriteGraphics(struct ActorAttachmentManagerView *manager,
                                                      s32 kind)
{
    switch (kind) {
        case 0:
            return ((struct ActorAttachmentManagerResources *)manager)->graphicsA;
        case 1:
        case 4:
        case 5:
            return ((struct ActorAttachmentManagerResources *)manager)->graphicsB;
        default:
            return NULL;
    }
}
static inline struct ActorAttachmentFollowView *
ActorAttachment_GetFollow(struct ActorAttachmentSlotView *slot)
{
    return slot->followActor;
}
static inline void ActorAttachment_SetupSprite(struct ActorAttachmentManagerView *manager,
                                               struct ActorAttachmentSlotView *slot,
                                               struct ContactPosition *anchor)
{
    struct ContactSwarmSprite *sprite;
    void *graphics;
    void *sequence;
    s32 xOffset, zOffset;
    zOffset = 0;
    xOffset = 0;
    sprite = &slot->sprite;
    graphics = ActorAttachment_GetSpriteGraphics(manager, slot->kind);
    Sprite_Init(sprite, graphics, 0);
    if (slot->action == 255) {
        slot->offset = ActorAttachment_GetFollow(slot)->offset;
        sprite->flags |= 1;
    } else {
        ActorAttachment_GetActionOffset(
            slot->resourceKind, slot->action,
            ((struct ActorAttachmentFollowView *)slot->followActor)->direction, &xOffset, &zOffset,
            slot->source);
        slot->offset.x = xOffset;
        slot->offset.y = 0;
        slot->offset.z = zOffset;
        sprite->flags |= 1;
    }
    if (ActorAttachment_IsAlternateMode()) {
        sprite->flags |= 0x10;
        sprite->priority = 1;
    } else {
        sprite->priority = 1;
    }
    slot->sequences[0].unknownA = 255;
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
    Anim_SetSequence(&slot->sequences[0], sequence, slot->frame, 0, 0);
    {
        struct ContactSwarmSprite *placed = &slot->sprite;
        placed->position = *anchor;
    }
}
struct ActorAttachmentSlotView *ActorAttachment_CreateValueSpriteSlot(
    u32 frame, struct ContactPosition *anchor, void *update, s32 y, s32 z, u32 action, s16 *value,
    s32 count, u32 emitBurst, void *source, struct ActorAttachmentFollowView *follow)
{
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentSlotView *slot;
    struct ContactPosition burstPositions[3];
    u8 startFrame = frame;
    u16 thresholdCount = count;
    s32 i;
    u32 found;
    existing = gActorAttachmentManager;
    if (!existing) {
        manager = ActorAttachment_CreateManager();
        if (!manager)
            return NULL;
    } else
        manager = existing;
    if (!follow)
        return NULL;
    slot = ActorAttachment_AllocateSlotHeader(manager);
    if (!slot)
        return NULL;
    slot->frame = startFrame;
    slot->flags31 = 1;
    slot->position.y = y;
    slot->action = action;
    slot->position.z = z;
    slot->position.pad = z;
    slot->thresholds[0] = Div((s16)thresholdCount, 5);
    slot->active48 = 1;
    slot->followActor = follow;
    slot->value = value;
    found = 0;
    i = 4;
    if (*value >= slot->thresholds[0] * i) {
        slot->frame = 0;
    } else {
    loop:
        if (--i > 0) {
            if (*slot->value < slot->thresholds[0] * i)
                goto loop;
            slot->frame = 4 - i;
            found = 1;
        }
        if (!found)
            slot->frame = 3;
    }
    slot->update = update;
    slot->resourceKind = 1;
    slot->kind = 1;
    slot->anchor = anchor;
    slot->source = source;
    ActorAttachment_SetupSprite(manager, slot, anchor);
    slot->active48 = 1;
    if (!(gEngineFlags & 0x100) && emitBurst == 1) {
        struct ContactPosition *position;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        Mod(*(u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2), 3);
        position = burstPositions;
        for (i = 2; i >= 0; i--) {
            position->x = slot->anchor->x;
            position->y = slot->anchor->y + z;
            position->z = slot->anchor->z;
            position++;
        }
    }
    return slot;
}
