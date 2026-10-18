#include "global.h"
#define SceneSpriteCollider_Init sub_08180490
struct SceneSpriteCollider {
    u8 unknown0[8];
    u8 sprite[0x2c];
    u8 animation[0x26];
    u16 state;
    u8 collider[0x54];
};
struct ColliderExtent {
    u32 x : 16, y : 16, z : 16;
    u32 : 16;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void sub_08180488(void);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
static inline void ClearPosition(u32 value, void *dest)
{
    CpuSet(&value, dest, 0x05000002);
}
s32 sub_08180490(struct SceneSpriteCollider *p)
{
    struct ColliderExtent size, offset;
    u32 zero;
    s32 result;
    u32 clear;
    u8 *sprite = p->sprite;
    u8 *anim = p->animation;
    void *position;
    u8 *collider;
    AnimData_Load(anim, 0x2110);
    Sprite_Init(sprite, anim, 0x43);
    {
        u32 scale = 4;
        clear = 0;
        *(u16 *)(sprite + 0x10) = scale;
    }
    AnimData_SelectPaletteBank(anim, 0x51);
    sprite[7] = 1;
    sprite[9] = 0x7f;
    sprite[8] = 0x7f;
    position = sprite + 0x1c;
    ClearPosition(clear, position);
    collider = sprite + 0x54;
    size.x = 100;
    size.y = 100;
    size.z = 100;
    offset.x = clear;
    offset.y = clear;
    offset.z = clear;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, position, 0);
    Collider_SetCallback(collider, sub_08180488, p);
    result = Collider_SetAttackProperties(collider, 30, 10, 0, 2, 10);
    p->state = clear;
    return result;
}
