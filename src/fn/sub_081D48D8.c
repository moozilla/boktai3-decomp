#include "global.h"
#define PaletteTransition_StartDisplay sub_081D48D8
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
    u8 pad0[0x26];
    u16 timer;
    u8 anim[0x9c4];
    u16 origin[4];
    u8 pad9f4[0xa];
    u8 dirX;
    u8 dirY;
    u16 direction;
};
extern struct PaletteDisplayEntry gUnk_086126E0[];
s32 DisplayAnimation_InitializeKeyframes(void *, int, int, void *);
s32 DisplayAnimation_InitializeSequence(void *, int, int, int, void *);
void DisplayAnimation_SelectPaletteBank(void *, u32);
void DisplayAnimation_SetPriority(void *, u32);
void Dir_Decompose(u8, u8 *, u8 *, u32);
s32 PaletteTransition_StartDisplay(struct PaletteTransition *p, int index)
{
    void *anim = p->anim;
    struct PaletteDisplayEntry *e = &gUnk_086126E0[index];
    u16 t = p->timer;
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
    if (e->kind == 2) {
        if (DisplayAnimation_InitializeKeyframes(anim, e->id, 0x10, (u8 *)p + 0x9ec) < 0)
            return -1;
    } else {
        if (DisplayAnimation_InitializeSequence(anim, e->id, e->frames, 0x10, (u8 *)p + 0x9ec) < 0)
            return -1;
    }
    if (!(e->flags & 1))
        DisplayAnimation_SelectPaletteBank(anim, e->palette);
    DisplayAnimation_SetPriority(anim, 2);
    p->direction = e->direction;
    Dir_Decompose(e->directionIndex, (u8 *)p + 0x9fe, (u8 *)p + 0x9ff, p->direction);
    return 0;
}
