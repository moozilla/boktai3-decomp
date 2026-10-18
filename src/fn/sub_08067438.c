#include "scene_actor_control.h"
s32 Mod(s32, s32);
extern const u8 gUnk_08605B40[];
void Sprite_LinkToList(void *, u32);
void Sprite_UnlinkFromList(void *, u32);
void SceneActorControl_TickDeparture(struct SceneActorControlManager *p,
                                     struct SceneActorControlSlot *slot)
{
    s32 tier, frame;
    struct SceneActorControlSprite *sprite = (void *)slot;
    if (sprite->active == 0)
        Sprite_LinkToList(sprite, 0);
    if (slot->stateFrame == 18) {
        u16 *address = &slot->flags;
        u32 flags = *address;
        s32 mask = ~0x50;
        mask &= flags;
        *address = mask;
    }
    frame = slot->stateFrame;
    tier = 2;
    if (frame <= 60) {
        tier = 0;
        if (frame > 40)
            tier = 1;
    }
    if (Mod(slot->stateFrame, gUnk_08605B40[tier]) == 0) {
        if (sprite->flags & 1)
            sprite->flags &= ~1;
        else
            sprite->flags |= 1;
    }
    if (slot->stateFrame > 90) {
        p->used &= ~(1U << slot->index);
        slot->kind = 0;
        Collider_Remove(&slot->collider);
        Sprite_UnlinkFromList(sprite, 0);
        slot->index = -1;
        slot->flags = 0;
    } else
        slot->stateFrame++;
}
