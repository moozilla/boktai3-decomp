#include "actor_attachment_state.h"
void ActorAttachment_TickWideShake(void *unused, struct ActorAttachmentSlotView *slot)
{
    struct ContactSwarmSprite *sprite = &slot->sprite;
    if (slot->timer <= 0) {
        u32 one = 1;
        if (ActorAttachment_TestByteFlags(&slot->status[0], one)) {
            u8 *f = &slot->status[0];
            s32 m = -2;
            *f = *f & m;
        }
    } else {
        u32 one = 1;
        if (ActorAttachment_TestByteFlags(&slot->status[0], one)) {
            int r;
            u32 v;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            r = Mod(*(u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400), 0x14);
            v = (u16)sprite->position.x;
            v -= 10;
            sprite->position.x = v + r;
        }
        slot->timer--;
    }
}
