// MATCHING_CONCESSION: register-pinning
#include "global.h"
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u32, u32);
static inline int trunc1(int x)
{
    return x >= 0 ? x >> 1 : -(-x >> 1);
}
void HresvelgrChild_Init(u8 *parent, u8 *obj, u32 arg)
{
    u8 *anim = obj + 0x2c;
    s16 *out;
    struct {
        s16 x, y, z, pad;
    } v;
    s16 *vp;
    s32 a;
    register s32 b asm("r6");
    s32 t;
    u32 zero, k;
    AnimData_Load(anim, 0x89CA);
    *(u16 *)(anim + 6) = 0x6F;
    *(u8 **)(anim + 0xC) = parent + 0x718;
    Sprite_Init(obj, anim, 0);
    k = 0x18;
    zero = 0;
    *(u16 *)(obj + 0x10) = k;
    obj[7] = 2;
    *(void **)(obj + 0x74) = ScriptProc_Start(0x922E, 0x572B);
    *(u32 *)obj |= 1;
    *(u32 *)(obj + 0x7C) = arg;
    *(u32 *)(obj + 0x88) = zero;
    *(u32 *)(obj + 0x78) = zero;
    out = (s16 *)(obj + 0x80);
    *(u32 *)&v = zero;
    vp = &v.x;
    a = vp[0];
    b = v.y;
    t = ((a + b) * 32) / 3 + 0x40;
    out[0] = trunc1(t);
    out[1] = 0x40;
    t = ((b - a) * 32) / 3 + 0x40;
    out[2] = trunc1(t);
}
