#include "numeric_overlay.h"

u16 *sub_0800F4C4(u32 bg, u32 x, s32 y)
{
    struct BgMapInfo *t = &gUnk_03004C30[bg];
    u16 *p = t->map;
    p = p + x;
    return p + (t->width << 1) * y;
}

INCLUDE_ASM("asm/nonmatching", sub_0800F4EC);

void sub_0800F6B0(struct NumericOverlay *s)
{
    s32 a, b, c;
    u32 z;
    sub_082286E0(&a, &b, &c, sub_08228D7C());
    z = 0;
    s->dirty = 0xf;
    sub_0800F4EC(s);
    s->dirty = z;
}

// Returns the (unchanged) r0 of sub_0800F6B0.
extern u32 NumericOverlay_RefreshResult(struct NumericOverlay *) asm("sub_0800F6B0");
u32 sub_0800F6E0(struct NumericOverlay *p)
{
    return NumericOverlay_RefreshResult(p);
}

u32 sub_0800F6EC(void)
{
    gUnk_02000048 = 0;
}

INCLUDE_ASM("asm/nonmatching", sub_0800F6F8);

void *sub_0800F80C(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))sub_0800F6E0, (void (*)(void *))sub_0800F6EC);
        if (sub_0800F6F8(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
