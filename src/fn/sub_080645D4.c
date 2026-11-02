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
    if (!source)
        return;
    if (!SameOwner(&source->ownerKey))
        return;
    if (slot->flags32 & 1)
        return;
    if (source->category == 0x15)
        return;
    switch (source->state) {
        case 4:
            Sound_PlaySong(0xEE);
            slot->flags32 |= 1;
            break;
        case 1:
            if (slot->kind != 3) {
                Sound_PlaySong(0xFF);
                slot->flags32 |= 1;
            }
            break;
        case 3:
            SceneActorPlacement_PlayVariantEventSound(source, 3);
            slot->flags32 |= 1;
            break;
    }
}
