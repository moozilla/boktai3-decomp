#include "global.h"

struct PaletteDisplayEntry {
    u16 id;
    u16 frames;
    u16 kind;
    u16 f6;
    u32 direction;
    u16 palette;
    u8 directionIndex;
    u8 flags;
    u32 f10;
    u8 pad14[4];
    s8 offsetX;
    s8 offsetY;
    u8 pad1a[10];
};
struct PaletteTransition {
    u8 pad0[0x1c];
    s32 index;
    u8 pad20[6];
    u16 timer;
    u8 anim[6][0x94];
    u8 pad_a[0x9ec - 0x28 - 6 * 0x94];
    u16 origin[4];
    u8 pad9f4[8];
    u16 f9fc;
    u8 pad9fe[0xa08 - 0x9fe];
    struct PaletteDisplayEntry *entry;
};
extern struct PaletteDisplayEntry gUnk_086126E0[];
void DisplayAnimation_Destroy(void *);
s32 PaletteTransition_StartDisplay(struct PaletteTransition *p, int index);
void Callback_InvokeActor(void *, u32);

void PaletteTransition_Restart(struct PaletteTransition *p)
{
    struct PaletteDisplayEntry *e = &gUnk_086126E0[p->index];
    s32 i;
    u16 t;
    s8 k2 = 0;
    p->entry = e;
    for (i = 0; i <= 5; i++) {
        s32 off = i * 0x94;
        u8 *q = (u8 *)p + 0x28;
        if (*(u32 *)(q + off) != 0)
            DisplayAnimation_Destroy((u8 *)p + (off + 0x28));
    }
    p->f9fc = e->f6;
    t = p->timer;
    if (t == 0) {
        s32 x = e->offsetX;
        s32 y = e->offsetY;
        u16 *origin = (u16 *)((u8 *)p + 0x9ec);
        x += 0x40;
        y += 0x54;
        origin[k2] = x;
        origin[1] = y;
        origin[2] = t;
        origin[3] = t;
    }
    if (e->f10 == 0)
        PaletteTransition_StartDisplay(p, p->index);
    else
        Callback_InvokeActor(p, e->f10);
}
