#include "sprite_effect_controller.h"
struct SpriteEffectColliderVector {
    u32 x : 16, y : 16, z : 16;
};
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
void SpriteEffectController_IgnoreContact(void);
void SpriteEffectController_InitCollider(struct SpriteEffectController *controller, s32 a, s32 b,
                                         s32 c, s32 d)
{
    struct SpriteEffectColliderVector size, offset;
    struct ContactCollider *collider;
    size.x = 16;
    size.y = 32;
    size.z = 16;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    collider = &controller->collider;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    sub_0821FF58(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, SpriteEffectController_IgnoreContact, controller);
    if (controller->flags & 8)
        Collider_SetPosition(
            collider, &((struct SpriteEffectSequenceDrawable *)controller->drawable)->position, 0);
    else
        Collider_SetPosition(
            collider, &((struct SpriteEffectTileDrawable *)controller->drawable)->position, 0);
}
