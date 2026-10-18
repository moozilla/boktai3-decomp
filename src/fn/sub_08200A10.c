#include "global.h"

static inline void fill00A(const void *source, void *dest, u32 control)
{
    CpuSet(source, dest, control);
}

static inline u32 flag00A(u32 value)
{
    u32 mask = 4;
    mask |= value;
    return mask;
}

struct Vec00A {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

void PlayerClone_InitConfig(u8 *);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(u8 *, void *);
void sub_0821983C(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
u32 ActorDebris_Create(u8 *, u8 *, u8 *);
void PlayerEffect_SetPaletteSource(u32, u32, u32);
void ScaledShadow_InitializeDefault(u8 *, u8 *);
void ScaledShadow_Disable(u8 *);
void Collider_Init(u8 *, u32, u32, u32, u32, struct Vec00A *, struct Vec00A *);
void Collider_SetCallback(u8 *, void *, u8 *);
void Collider_SetBodyProperties(u8 *, s32, u32, u32);
s32 Collider_Refresh(u8 *);
void Collider_SetAttackProperties(u8 *, s32, s32, u32, u32, s32);
void PlayerClone_OnBodyColliderContact(void);
void PlayerClone_OnPassiveColliderContact(void);

void PlayerClone_InitSlot(u32 unused, u8 *s, u32 arg)
{
    u8 *context = s + 0x198;
    u32 zero, fill;
    u8 *res, *p;
    u32 value;
    struct Vec00A v, w;

    PlayerClone_InitConfig(context);
    zero = 0;
    *(u32 *)s = zero;
    {
        u8 *dest = s + 0xc;
        u32 *source;
        fill = zero;
        source = &fill;
        fill00A(source, dest, 0x05000002);
    }
    res = ScriptProc_Start(0xcb05, 0xde23);
    {
        u8 *obj = s + 0x278;
        ResourceHeader_Relocate(obj, res);
        {
            u8 *p = s + 0x1b8;
            sub_0821983C(p, obj, 0, 0, 2, 1, zero, zero);
            *(u32 *)(s + 0x1c0) |= 1;
            value = ActorDebris_Create(p, s + 0x320, s + 0x322);
            *(u32 *)(s + 0x33c) = value;
            PlayerEffect_SetPaletteSource(value, 0xe001, 0);
        }
    }
    {
        u8 *obj = s + 0x298;
        ScaledShadow_InitializeDefault(obj, s + 0x1d8);
        ScaledShadow_Disable(obj);
    }
    *(u32 *)(s + 0x328) = arg;
    p = s + 0x80;
    v.x = 40;
    v.y = 128;
    v.z = 40;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x4001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnBodyColliderContact, s);
    Collider_SetBodyProperties(p, *(s16 *)(context + 0x16), 0x402, 1);
    Collider_Refresh(p);
    *(u16 *)(p + 6) = flag00A(*(u16 *)(p + 6));
    p -= 0x54;
    v.x = 64;
    v.y = 128;
    v.z = 64;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
    Collider_SetAttackProperties(p, *(s16 *)(context + 0x10), *(s16 *)(context + 0x12), 0, zero,
                                 *(s16 *)(context + 0x14));
    p += 0xa8;
    v.x = 200;
    v.y = 128;
    v.z = 200;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
    Collider_SetAttackProperties(p, *(s16 *)(context + 0x18), *(s16 *)(context + 0x1a), 0, zero,
                                 *(s16 *)(context + 0x1c));
    p = s + 0x128;
    v.x = 32;
    v.y = 32;
    v.z = 32;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
}
