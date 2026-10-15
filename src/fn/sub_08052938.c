#include "stacked_actor.h"
#define StackedActor_Appear sub_08052938
void AnimData_SelectTiles(void *, u32);
void StackedActor_Appear(struct StackedActor *actor)
{
    u16 *frame = &actor->frame;
    u32 value;
    {
        u32 next = *frame + 1;
        *frame = next;
        value = (u16)next;
    }
    if (value <= 63) {
        actor->sprite.flags = (actor->sprite.flags & ~1) | 2;
        actor->shadow.flags |= 2;
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 2);
        {
            u32 scale = *frame;
            actor->sprite.scaleX = scale;
            actor->sprite.scaleY = scale;
        }
        actor->sprite.rotation = *(u8 *)frame << 2;
        actor->shadow.scaleX = actor->sprite.scaleX;
        actor->shadow.scaleY = actor->sprite.scaleX;
        actor->shadow.rotation = actor->sprite.rotation;
        actor->sprite.position.y = (u16)actor->world.y + 22;
    } else if (value == 64) {
        actor->sprite.flags &= ~2;
        actor->shadow.flags &= ~2;
        actor->sprite.scaleX = value;
        actor->sprite.scaleY = value;
        actor->shadow.scaleX = value;
        actor->shadow.scaleY = value;
    } else if (value > 79) {
        actor->sprite.position.y--;
        if (actor->sprite.position.y > actor->world.y)
            AnimData_SelectTiles(actor->graphics, actor->tileBase + 1);
        else {
            AnimData_SelectTiles(actor->graphics, actor->tileBase);
            actor->sprite.flags |= 1;
            sub_08052628(actor, sub_08052A40);
        }
    }
}
