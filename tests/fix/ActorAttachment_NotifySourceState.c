// draft with the flag set once after the switch. The ROM keeps the flag test's 1 in r5 for it: the
// original set the flag in each case, which cse enters knowing the 1 (sp_tail_into_branches, README fact 65)
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
    slot->flags32 |= 1;
}
