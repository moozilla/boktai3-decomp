#include "global.h"
#include "engine.h"
struct PackedVector {
    s32 x : 16, y : 16, z : 16;
};
struct SpriteEffect {
    u8 pad0[0x2c];
    u32 position[2];
    u8 pad34[0x3c - 0x34];
    u8 collider[0x44];
};
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SceneActor14_OnColliderContact(void *, void *, struct SpriteEffect *);
void SpriteEffect_InitCollider(struct SpriteEffect *p, s32 damage, s32 knockback, s32 element,
                               s32 extra)
{
    struct PackedVector size, offset;
    void *c;
    size.x = 16;
    size.y = 16;
    size.z = 16;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    c = p->collider;
    Collider_Init(c, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetAttackProperties(c, damage, knockback, 0, element, extra);
    Collider_SetPosition(c, &p->position, 0);
    Collider_SetCallback(c, SceneActor14_OnColliderContact, p);
}
