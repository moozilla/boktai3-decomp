// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_slot.h"
#define ActorAttachment_NotifySourceState sub_080645D4
void sub_08077848(struct ActorAttachmentSource *, u32);
static inline u8 SameOwner(u16 *key)
{
    if (gUnk_02000710->region == *key)
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
            sub_08077848(source, 3);
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
