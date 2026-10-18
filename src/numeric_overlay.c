#include "numeric_overlay.h"

u16 *NumericOverlay_GetTileMapPtr(u32 bg, u32 x, s32 y)
{
    struct BgMapInfo *t = &gUnk_03004C30[bg];
    u16 *p = t->map;
    p = p + x;
    return p + (t->width << 1) * y;
}

INCLUDE_ASM("asm/nonmatching", sub_0800F4EC);

void NumericOverlay_RefreshDisplay(struct NumericOverlay *s)
{
    s32 a, b, c;
    u32 z;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    z = 0;
    s->dirty = 0xf;
    sub_0800F4EC(s);
    s->dirty = z;
}

// Returns the (unchanged) r0 of NumericOverlay_RefreshDisplay.
extern u32 NumericOverlay_RefreshResult(struct NumericOverlay *) asm("NumericOverlay_RefreshDisplay");
u32 NumericOverlay_Update(struct NumericOverlay *p)
{
    return NumericOverlay_RefreshResult(p);
}

u32 NumericOverlay_Destroy(void)
{
    gUnk_02000048 = 0;
}

INCLUDE_ASM("asm/nonmatching", sub_0800F6F8);

void *NumericOverlay_Create(u32 arg)
{
    void *obj;
    obj = Task_Create(9, 0x2C);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))NumericOverlay_Update, (void (*)(void *))NumericOverlay_Destroy);
        if (sub_0800F6F8(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
