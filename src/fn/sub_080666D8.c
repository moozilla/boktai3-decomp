// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
static inline u8 AlternateMode(void)
{
    u32 bit = 0x80;
    {
        struct GameState *base = gGameState;
        register u32 offset asm("r3");
        u32 *flags;
        offset = 0x868;
        asm("" : "+r"(offset));
        flags = (u32 *)((u32)base + offset);
        if (*flags & bit)
            return TRUE;
    }
    return FALSE;
}
struct ActorAttachmentSlotView *
ActorAttachment_CreateFixedSpriteSlot(struct ContactPosition *anchor, void *update, s32 y, s32 z,
                                      u32 action, void *source,
                                      struct ActorAttachmentFollowView *follow)
{
    register void *savedUpdate asm("r8") = update;
    s32 savedY = y;
    s32 savedZ = z;
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentManagerView *existing;
    struct ActorAttachmentSlotView *slot;
    register u8 *framePointer asm("r10");
    register u8 *actionPointer asm("r5");
    struct ContactSwarmSprite *spritePointer;
    u8 *activePointer;
    u8 *activeLow;
    u8 *kindLow;
    u8 *resourceLow;
    u8 *resourcePointer;
    register u8 *kindPointer asm("r6");
    struct ContactSwarmSprite *sprite;
    void *graphics, *sequence;
    s32 xOffset, zOffset;
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
    if (!follow)
        goto failed;
    slot = ActorAttachment_AllocateSlotHeader(manager);
    if (!slot)
        goto failed;
    {
        u32 offset = 0x30;
        u8 *low;
        asm("" : "+r"(offset));
        low = (u8 *)(offset + (u32)slot);
        framePointer = low;
        {
            u32 zero = 0;
            u32 frame = 5;
            asm("" : "+r"(zero), "+r"(frame));
            *low = frame;
            slot->flags31 = 1;
            slot->position.y = savedY;
            actionPointer = &slot->action;
            *actionPointer = action;
            slot->position.z = savedZ;
            slot->position.pad = savedZ;
            activeLow = &slot->active48;
            *activeLow = 1;
            slot->unknown34 = zero;
            {
                register void *low asm("r3") = savedUpdate;
                asm("" : "+r"(low));
                slot->update = low;
            }
            resourceLow = &slot->resourceKind;
            *resourceLow = 4;
            kindLow = &slot->kind;
            *kindLow = 4;
            {
                register struct ContactPosition *low asm("r6") = anchor;
                asm("" : "+r"(low));
                slot->anchor = low;
            }
            slot->source = source;
            {
                u32 offset = 0x134;
                struct ActorAttachmentFollowView **dst;
                dst = (void *)((u32)slot + offset);
                {
                    struct ActorAttachmentFollowView *low = follow;
                    *dst = low;
                }
            }
            zOffset = zero;
            xOffset = zero;
        }
    }
    {
        u32 offset = 0x60;
        sprite = (void *)((u32)slot + offset);
    }
    {
        s32 kind = *kindLow;

        activePointer = activeLow;
        resourcePointer = resourceLow;
        kindPointer = kindLow;
        spritePointer = sprite;
        if (kind == 1)
            goto graphicsB;
        if (kind > 1)
            goto graphicsRange;
        if (kind == 0)
            goto graphicsA;
        goto graphicsNone;
    graphicsRange:
        switch (kind) {
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
    if (*actionPointer == 255) {
        register u32 offset asm("r2") = 0x134;
        register struct ActorAttachmentFollowView **target asm("r0");
        asm("" : "+r"(offset));
        target = (void *)((u32)slot + offset);
        slot->offset = (*target)->offset;
    } else {
        ActorAttachment_GetActionOffset(
            *resourcePointer, *actionPointer,
            ((struct ActorAttachmentFollowView *)slot->followActor)->direction, &xOffset, &zOffset,
            slot->source);
        slot->offset.x = xOffset;
        slot->offset.y = 0;
        slot->offset.z = zOffset;
    }
    {
        struct ContactSwarmSprite *low = sprite;
        register u32 flags asm("r0");
        register struct ContactSwarmSprite *dst asm("r2");
        flags = low->flags;
        flags |= 1;
        dst = sprite;
        asm("" : "+r"(dst));
        dst->flags = flags;
    }
    if (AlternateMode()) {
        register struct ContactSwarmSprite *low asm("r1") = sprite;
        register u32 flags asm("r0");
        register struct ContactSwarmSprite *dst asm("r2");
        asm("" : "+r"(low));
        flags = low->flags;
        flags |= 0x10;
        dst = sprite;
        asm("" : "+r"(dst));
        dst->flags = flags;
    }
    {
        u32 one = 1;
        struct ContactSwarmSprite *low;
        low = sprite;
        low->priority = one;
    }
    {
        u32 offset = 0x10E;
        u16 *p;
        p = (void *)((u32)slot + offset);
        *p = 255;
    }
    switch (*kindPointer) {
        case 0: {
            struct ActorAttachmentManagerResources *low = (void *)manager;
            sequence = low->sequenceA;
            break;
        }
        case 2:
        case 3: {
            register struct ActorAttachmentManagerResources *low asm("r2") = (void *)manager;
            asm("" : "+r"(low));
            sequence = low->sequenceB;
            break;
        }
        case 1:
        case 4:
        case 5:
        default:
            sequence = 0;
            break;
    }
    {
        u32 offset = 0x104;
        struct DisplaySequenceState *state;
        state = (void *)((u32)slot + offset);
        Anim_SetSequence(state, sequence, *framePointer, 0, 0);
    }
    {
        register struct ContactPosition *low asm("r2") = anchor;
        struct ContactPosition position;
        register struct ContactSwarmSprite *dst asm("r3");
        asm("" : "+r"(low));
        position = *low;
        dst = spritePointer;
        asm("" : "+r"(dst));
        dst->position = position;
    }
    {
        u32 one = 1;
        register u8 *low asm("r6");
        low = activePointer;
        asm("" : "+r"(low));
        *low = one;
    }
    return slot;
}
