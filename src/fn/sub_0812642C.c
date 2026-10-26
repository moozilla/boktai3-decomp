#include "global.h"
#include "ram.h"
// fix.py best version (reprinted C)
extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
u8 *ScriptedParticle_EnsurePool(void);
u8 *ScriptedParticle_AllocateSlot(u8 *);
void AnimData_Load(u8 *, u32);
void *ScriptProc_Start(u32, u32);
void Sprite_Init(u8 *, u8 *, u32);
void Anim_SetSequence(u8 *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(u8 *, u32);
s32 Mod(s32, s32);
void SpriteParticle_Create(u32 *a, s32 b, s32 c, s32 d, s32 e, u32 f, s32 g, s32 h, u32 i, u32 j,
                           u32 k, u32 l, u32 m, u32 n, u32 o, u32 p, u32 q, u32 r)
{
    struct Pair {
        u32 x;
        u32 y;
    };
    u8 *context = gScriptedParticlePool;
    u8 *s;
    u8 *resource;
    u8 *fields;
    u32 zero;
    u16 *table;
    u32 *rng;
    u32 mask;
    s32 value;
    typeof((Mod(table[*rng], e) - (e >> 1)) + d) t1;
    typeof(Mod(table[*rng], c) + b) t2;
    if (!context) {
        context = ScriptedParticle_EnsurePool();
        if (!context)
            goto end;
    }
    fields = (s = ScriptedParticle_AllocateSlot(context));
    if (!s)
        goto end;
    fields += 0x58;
    zero = 0;
    {
        u8 k1 = 1;
        *fields = k1;
    }
    fields += 0xE;
    *((u16 *)fields) = zero;
    *((struct Pair *)(s + 0x1C)) = *((struct Pair *)a);
    resource = s + 0x2C;
    AnimData_Load(resource, n);
    *((void **)(context + 0x18)) = ScriptProc_Start(0x922E, (u16)o);
    Sprite_Init(s, resource, 0);
    Anim_SetSequence(s + 0x48, *((void **)(context + 0x18)), (u16)p, (u8)q, zero);
    AnimData_SelectPaletteBank(resource, r);
    table = gUnk_0203B400;
    rng = &gRandomIndex;
    mask = 0x3FF;
    *rng = ((*rng) + 1) & 0x3FF;
    t2 = Mod(table[*rng], c) + b;
    value = t2;
    *((u16 *)(s + 0x5C)) = value;
    *rng = ((*rng) + 1) & 0x3FF;
    t1 = (Mod(table[*rng], e) - (e >> 1)) + d;
    value = t1;
    *((u16 *)(s + 0x5E)) = value;
    *rng = ((*rng) + 1) & 0x3FF;
    value = g + Mod(table[*rng], h);
    s[0x5A] = value;
    *((u16 *)(s + 0x64)) = f;
    *((u16 *)(s + 0x68)) = i;
    s[0x6D] = 12;
    *((u32 *)s) |= 1 | l;
    s[7] = j;
    *((u16 *)(s + 0x6E)) = m;
end:;

    ;
    ;
    ;
    ;
    ;
}
