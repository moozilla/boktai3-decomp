#include "global.h"
#include "ram.h"
struct Slot {
    u8 bytes[0x38];
};
struct S {
    u8 p0[0x18];
    u16 x, y, z, p1E;
    struct Slot slots[4];
    void *res;
};
void *AnimData_FindEntry(u32);
void ScreenOrbitEffect_InitSlot(void *, void *);
u32 ScreenOrbitEffect_Init(struct S *s)
{
    struct Slot *p;
    int i;
    gEffectQuad = s;
    *(void **)((u8 *)s + 0x100) = AnimData_FindEntry(0x1c1a);
    s->x = 0x78;
    s->y = 0x50;
    s->z = 0;
    p = s->slots;
    i = 3;
    do {
        ScreenOrbitEffect_InitSlot(s, p);
        p++;
        i--;
    } while (i >= 0);
    return 0;
}
