#include "actor_attachment_state.h"
struct ActorAttachmentFollowView {
    u8 unknown0[9];
    u8 direction;
    u8 unknownA[0x2A];
    struct ContactPosition offset;
};
extern void *const gUnk_08605B18[];
void Effect_SetOrigin(struct ActorAttachmentPartSprite *, s32, s32);
void ActorAttachment_UpdateFollowingOffset(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentFollowView *follow = slot->followActor;
    if (follow) {
        s32 x, z;
        if (slot->action == 0xFF)
            slot->offset = follow->offset;
        else {
            u8 *kind = &slot->resourceKind;
            s32 index;
            struct ActorAttachmentPartSprite *part;
            u8 *mask;
            ActorAttachment_GetActionOffset(*kind, slot->action, follow->direction, &x, &z,
                                            slot->source);
            if ((u8)(*kind - 2) <= 1) {
                index = 0;
                mask = &slot->partMask;
                kind += 0x4D;
                part = (void *)kind;
                for (; index <= 2; index++) {
                    if ((*mask >> index) & 1)
                        Effect_SetOrigin(part, x, z);
                    part++;
                }
            } else {
                s32 copyX = x;
                s32 y = 0;
                slot->offset.x = copyX;
                slot->offset.y = y;
                slot->offset.z = z;
            }
        }
    }
    sub_0824923C(slot, gUnk_08605B18[slot->resourceKind]);
}
