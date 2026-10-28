// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
static inline u8 Any(u32 bits)
{
    if (bits)
        return TRUE;
    return FALSE;
}
static inline u8 AlternateMode(void)
{
    u32 bit = 0x80;
    if (gGameState->flags868 & bit)
        return TRUE;
    return FALSE;
}
struct ActorAttachmentSlotView *
ActorAttachment_CreateSpriteSlot(u32 frame, struct ContactPosition *anchor, void *update, s32 y,
                                 s32 z, u32 action, void *source,
                                 struct ActorAttachmentFollowView *follow)
{
    void *savedUpdate;
    s32 savedY;
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentSlotView *slot;
    u32 startFrame;
    register struct ActorAttachmentFollowView *savedFollow asm("r5");
    u8 *framePointer, *activePointer;
    u8 *frameLow;
    u8 *activeLow;
    u8 *kindPointer;
    u8 *actionPointer;
    u8 *savedKind;
    u8 *savedAction;
    struct ContactSwarmSprite *sprite;
    struct ContactSwarmSprite *savedSprite;
    u8 *resourcePointer;
    void *graphics;
    void *sequence;
    s32 xOffset, zOffset;
    savedUpdate = update;
    savedY = y;
    savedFollow = follow;
    startFrame = (u8)frame;
    existing = gActorAttachmentManager;
    if (!existing) {
        manager = ActorAttachment_CreateManager();
        if (manager)
            goto gotManager;
    failed:
        return 0;
    } else
        manager = existing;
gotManager:
    if (!savedFollow)
        goto failed;
    slot = ActorAttachment_AllocateSlotHeader(manager);
    if (!slot)
        goto failed;
    slot->followActor = savedFollow;
    frameLow = &slot->frame;
    {
        u32 zero = 0;
        *frameLow = startFrame;
        slot->flags31 = 1;
        kindPointer = &slot->kind;
        *kindPointer = zero;
        actionPointer = &slot->action;
        *actionPointer = action;
        slot->unknown34 = zero;
    }
    slot->update = savedUpdate;
    slot->position.y = savedY;
    slot->position.z = z;
    slot->status[0] = 1;
    slot->timer = 10;
    {
        u32 mask = 0x80;
        u32 bits;
        bits = gGameState->flags868 & mask;
        framePointer = frameLow;
        savedKind = kindPointer;
        savedAction = actionPointer;
        {
            u32 alternate = Any(bits);
            if (alternate) {
                u8 *low = &slot->resourceKind;
                *low = 6;
                resourcePointer = low;
            } else {
                u8 *low = &slot->resourceKind;
                *low = alternate;
                resourcePointer = low;
            }
        }
    }
    activeLow = &slot->active48;
    {
        u32 zero = 0;
        *activeLow = 1;
        slot->anchor = anchor;
        slot->source = source;
        zOffset = zero;
        xOffset = zero;
    }
    sprite = &slot->sprite;
    {
        u8 *lowKind = savedKind;
        s32 kind = *lowKind;
        s32 copy;
        copy = kind;
        activePointer = activeLow;
        savedSprite = sprite;
        if (kind == 1)
            goto graphicsB;
        if (kind > 1)
            goto graphicsRange;
        if (kind == 0)
            goto graphicsA;
        goto graphicsNone;
    graphicsRange:
        switch (copy) {
            case 4:
            case 5:
                goto graphicsB;
            default:
                goto graphicsNone;
        }
    graphicsA:
        graphics = ((struct ActorAttachmentManagerResources *)manager)->graphicsA;
        goto graphicsReady;
    graphicsB:
        graphics = ((struct ActorAttachmentManagerResources *)manager)->graphicsB;
        goto graphicsReady;
    graphicsNone:
        graphics = 0;
    graphicsReady:;
    }
    Sprite_Init(sprite, graphics, 0);
    {
        register u8 *lowAction asm("r2") = savedAction;
        {
            u32 actionValue = *lowAction;
            asm("" : "+r"(actionValue), "+r"(lowAction));
            if (actionValue == 255) {
                register u32 offset asm("r3") = 0x134;
                register struct ActorAttachmentFollowView **target asm("r0");
                asm("" : "+r"(offset));
                target = (struct ActorAttachmentFollowView **)((u32)slot + offset);
                slot->offset = (*target)->offset;
            } else {
                u32 resourceKind = *resourcePointer;
                lowAction = savedAction;
                ActorAttachment_GetActionOffset(
                    resourceKind, *lowAction,
                    ((struct ActorAttachmentFollowView *)slot->followActor)->direction, &xOffset,
                    &zOffset, slot->source);
                slot->offset.x = xOffset;
                slot->offset.y = 0;
                slot->offset.z = zOffset;
            }
        }
    }
    sprite->flags |= 1;
    if (AlternateMode())
        sprite->flags |= 0x10;
    sprite->priority = 1;
    slot->sequences[0].unknownA = 255;
    {
        u8 *lowKind = savedKind;
        switch (*lowKind) {
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
                sequence = 0;
                break;
        }
    }
    {
        u32 offset = 0x104;
        struct DisplaySequenceState *state;
        state = (struct DisplaySequenceState *)((u32)slot + offset);
        Anim_SetSequence(state, sequence, *framePointer, 0, 0);
    }
    {
        struct ContactPosition *lowAnchor = anchor;
        {
            struct ContactPosition position = *lowAnchor;
            {
                struct ContactSwarmSprite *lowSprite = savedSprite;
                lowSprite->position = position;
            }
        }
    }
    {
        u32 one = 1;
        u8 *lowActive;
        lowActive = activePointer;
        *lowActive = one;
    }
    return slot;
}
