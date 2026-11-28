#include "scene/actor/actor_attachment_manager.h"
extern void *const gUnk_08605B18[];
void Effect_SetOrigin(struct ActorAttachmentPartSprite *, s32, s32);
void ActorAttachment_UpdateFollowingOffset(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentFollowView *follow = slot->followActor;

    if (follow) {
        s32 x, z;

        if (slot->action == 0xFF) {
            slot->offset = follow->offset;
        } else {
            ActorAttachment_GetActionOffset(slot->resourceKind, slot->action, follow->direction, &x,
                                            &z, slot->source);
            if (slot->resourceKind == 2 || slot->resourceKind == 3) {
                s32 i;

                for (i = 0; i <= 2; i++) {
                    if ((slot->partMask >> i) & 1)
                        Effect_SetOrigin(&slot->parts[i], x, z);
                }
            } else {
                slot->offset.x = x;
                slot->offset.y = 0;
                slot->offset.z = z;
            }
        }
    }
    Callback_InvokeActor(slot, gUnk_08605B18[slot->resourceKind]);
}
