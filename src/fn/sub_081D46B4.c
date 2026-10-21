#include "global.h"
struct PaletteDisplayEntry {
    u16 id;
    u16 frames;
    u16 kind;
    u8 pad6[2];
    u32 direction;
    u16 palette;
    u8 directionIndex;
    u8 flags;
    u8 pad10[8];
    s8 offsetX;
    s8 offsetY;
    u8 pad1a[10];
};
struct PaletteTransition {
    u8 pad0[0x1c];
    u32 index;
    u8 pad20[6];
    u16 timer;
    u8 anim[0x94 * 2];
};
extern struct PaletteDisplayEntry gUnk_086126E0[];
void DisplayAnimation_InitializeKeyframes(void *, int, int, void *);
void DisplayAnimation_SetPriority(void *, u32);
void Dir_Decompose(u8, u8 *, u8 *, u32);
void PaletteTransition_InitDisplays(struct PaletteTransition *p)
{
    u8 *anim = p->anim;
    struct PaletteDisplayEntry *e = &gUnk_086126E0[p->index];
    u16 t = p->timer;
    s32 i;
    u16 *dir;
    u32 new_var;
    if (t == 0) {
        s32 x = e->offsetX;
        s32 y = e->offsetY;
        u16 *origin = (u16 *)((u8 *)p + 0x9ec);
        x += 0x40;
        y += 0x54;
        origin[0] = x;
        origin[1] = y;
        origin[2] = t;
        origin[3] = t;
    }
    for (i = 0; i < 2; i++) {
        u8 *a = anim + i * 0x94;
        DisplayAnimation_InitializeKeyframes(a, e->id, 0x10, (u8 *)p + 0x9ec);
        DisplayAnimation_SetPriority(a, 2);
    }
    new_var = e->direction;
    dir = (u16 *)((u8 *)p + 0xa00);
    *dir = new_var;
    Dir_Decompose(e->directionIndex, (u8 *)p + 0x9fe, (u8 *)p + 0x9ff, *dir);
}
