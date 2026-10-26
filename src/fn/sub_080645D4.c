// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_slot.h"
void SceneActorPlacement_PlayVariantEventSound(struct ActorAttachmentSource *, u32);
static inline u8 SameOwner(u16 *key)
{
    if (gGameState->region == *key)
        return TRUE;
    return FALSE;
}
void ActorAttachment_NotifySourceState(struct ActorAttachmentSlot *slot)
{
    struct ActorAttachmentSource *source = slot->source;
    u8 *flags;
    u32 one;
    register u32 bits asm("r1");
    if (!source)
        return;
    if (!SameOwner(&source->ownerKey))
        return;
    flags = &slot->flags32;
    bits = *flags;
    one = 1;
    asm("" : "+r"(one), "+r"(bits));
    if (bits & 1)
        return;
    if (source->category == 0x15)
        return;
    switch (source->state) {
        case 4:
            Sound_PlaySong(0xEE);
            break;
        case 1:
            if (slot->kind == 3)
                return;
            Sound_PlaySong(0xFF);
            break;
        case 3:
            SceneActorPlacement_PlayVariantEventSound(source, 3);
            break;
        default:
            return;
    }
    {
        u32 updated = *flags;
        updated |= one;
        *flags = updated;
    }
}
