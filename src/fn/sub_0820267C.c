#include "global.h"
struct Data {
    u8 pad0[0x14];
    u32 w14, w18;
    s16 h1c, h1e, h20, h22;
    u8 pad24[0x10];
    u32 w34;
};
void ProjectileConfig_Init(void *);
void AnimData_Load(void *, u32);
void Sprite_InitUnlinked(void *, void *, u32);
void Collider_Init(void *, u32, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void TimedSpriteEffect_OnColliderContact(void);
static inline void Fill32(void *dst, u32 size)
{
    u32 tmp = 0;
    CpuSet(&tmp, dst, 0x05000000 | size / 4);
}
void TimedSpriteEffect_InitSlot(void *unused, u8 *p)
{
    struct Data *data = (struct Data *)(p + 0x28);
    void *obj, *effect;
    ProjectileConfig_Init(data);
    *(u32 *)p = 0;
    Fill32(p + 0x10, 8);
    obj = p + 0x84;
    AnimData_Load(obj, data->w34);
    Sprite_InitUnlinked(p + 0xa0, obj, 1);
    effect = p + 0xcc;
    Collider_Init(effect, 0, 0x2001, 0, 16, p + 0x4c, p + 0x54);
    Collider_SetCallback(effect, TimedSpriteEffect_OnColliderContact, p);
    Collider_SetAttackProperties(effect, data->h1c, data->h1e, data->w14, data->w18, data->h22);
}
