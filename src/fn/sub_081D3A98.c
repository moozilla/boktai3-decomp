#include "global.h"
#include "engine.h"
struct PaletteTransitionTarget {
    u8 pad0[0xe];
    u8 direction;
};
struct PaletteTransition {
    u8 pad0[0x18];
    s32 timer;
    u8 pad1c[0xc];
    u8 anim[0x94];
    u8 pad_bc[0x9fc - 0xbc];
    u16 index;
    u8 dirX;
    u8 dirY;
    u16 scale;
    u8 pad_a02[6];
    struct PaletteTransitionTarget *target;
};
void Dir_Decompose(u8, u8 *, u8 *, u32);
int DisplayAnimation_SetEntry(void *, int, int, int, int, int, int);
int DisplayAnimation_UpdatePlayback(void *);
int DisplayAnimation_TestFinished(void *);
s32 Mod(s32, s32);
void PaletteTransition_UpdateBlink(struct PaletteTransition *p)
{
    s32 t = p->timer;
    if (t == 0) {
        u16 *index = &p->index;
        s32 one = 1;
        *index = one;
        Dir_Decompose(p->target->direction, &p->dirX, &p->dirY, p->scale);
        DisplayAnimation_SetEntry(p->anim, *index, p->dirX, p->dirY, one, t, 5);
        DisplayAnimation_UpdatePlayback(p->anim);
        if (DisplayAnimation_TestFinished(p->anim)) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            p->timer = Mod(*(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1)), 0x20) + 0x3c;
        }
    } else {
        p->timer = t - 1;
    }
}
