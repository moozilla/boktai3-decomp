#include "actor_attachment_manager.h"
static inline u8 ActorAttachment_IsAlternateMode(void)
{
    u32 bit = 0x80;
    if (gGameState->flags868 & bit)
        return TRUE;
    return FALSE;
}
static inline void *ActorAttachment_GetSpriteGraphics(struct ActorAttachmentManagerView *manager, s32 kind)
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
static inline struct ActorAttachmentFollowView *ActorAttachment_GetFollow(struct ActorAttachmentSlotView *slot)
{
    return slot->followActor;
}
struct ActorAttachmentSlotView *
ActorAttachment_CreateSpriteSlot(u32 frame, struct ContactPosition *anchor, void *update, s32 y,
                                 s32 z, u32 action, void *source,
                                 struct ActorAttachmentFollowView *follow)
{
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentSlotView *slot;
    struct ContactSwarmSprite *sprite;
    void *graphics;
    void *sequence;
    s32 xOffset, zOffset;
    u8 startFrame = frame;
    u8 alternate;
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
    slot->followActor = follow;
    slot->frame = startFrame;
    slot->flags31 = 1;
    slot->kind = 0;
    slot->action = action;
    slot->unknown34 = 0;
    slot->update = update;
    slot->position.y = y;
    slot->position.z = z;
    slot->status[0] = 1;
    slot->timer = 10;
    alternate = ActorAttachment_IsAlternateMode();
    if (alternate)
        slot->resourceKind = 6;
    else
        slot->resourceKind = alternate;
    slot->active48 = 1;
    slot->anchor = anchor;
    slot->source = source;
    zOffset = 0;
    xOffset = 0;
    sprite = &slot->sprite;
    graphics = ActorAttachment_GetSpriteGraphics(manager, slot->kind);
    Sprite_Init(sprite, graphics, 0);
    if (slot->action == 255) {
        slot->offset = ActorAttachment_GetFollow(slot)->offset;
    } else {
        ActorAttachment_GetActionOffset(slot->resourceKind, slot->action,
                                        ((struct ActorAttachmentFollowView *)slot->followActor)->direction,
                                        &xOffset, &zOffset, slot->source);
        slot->offset.x = xOffset;
        slot->offset.y = 0;
        slot->offset.z = zOffset;
    }
    sprite->flags |= 1;
    if (ActorAttachment_IsAlternateMode())
        sprite->flags |= 0x10;
    sprite->priority = 1;
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
    slot->active48 = 1;
    return slot;
}
