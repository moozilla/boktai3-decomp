#include "global.h"
struct StateSprite {
    u8 unk0[0xD4];
    s16 position[4];
    u8 collider[0x42];
    u8 collisionFlags;
};
struct ColliderVector {
    s16 x, y, z;
};
extern const struct ColliderVector gUnk_08610710, gUnk_08610718, gUnk_08610720;
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SceneScheduledEffect_OnColliderContact(void);
void StateSprite_InitCollider(struct StateSprite *sprite, s32 side, s32 type, s32 a, s32 b, s32 c)
{
    struct ColliderVector size, offset;
    u8 *collider;
    if (side == 0) {
        switch (type) {
            case 0:
                size = gUnk_08610710;
                break;
            case 1:
                size = gUnk_08610718;
                break;
            case 2:
                size = gUnk_08610720;
                break;
        }
        offset.x = 0;
        offset.y = 0;
        offset.z = size.z;
        side = 0x40;
    } else {
        switch (type) {
            case 0:
                size.x = gUnk_08610710.z;
                size.y = gUnk_08610710.y;
                size.z = gUnk_08610710.x;
                break;
            case 1:
                size.x = gUnk_08610718.z;
                size.y = gUnk_08610718.y;
                size.z = gUnk_08610718.x;
                break;
            case 2:
                size.x = gUnk_08610720.z;
                size.y = gUnk_08610720.y;
                size.z = gUnk_08610720.x;
                break;
        }
        offset.x = size.x;
        offset.y = 0;
        offset.z = 0;
        side = 0;
    }
    collider = sprite->collider;
    Collider_Init(collider, 0, 0x2101, 0, 0x20, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0x400200, 0x200004, c);
    Collider_SetPosition(collider, sprite->position, 0);
    Collider_SetCallback(collider, SceneScheduledEffect_OnColliderContact, sprite);
    sprite->collisionFlags = side;
}
