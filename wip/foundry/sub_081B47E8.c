#define ScenePaletteActor_UpdateInterpolation sub_081B47E8
#include "global.h"
struct ScenePaletteActor {
    u8 pad0[0x38];
    void *palette;
    u8 pad3C[0x124];
    u32 flags, state, timer;
    u8 pad16C[2];
    u8 reset, entered, finished;
    u8 pad171[0x1F];
    void *source;
    u8 pad194[4];
    void *target;
};
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void sub_081B47E8(void *unused, struct ScenePaletteActor *p, u32 phase)
{
    u32 mask = 12;
    p->flags |= mask;
    p->flags |= 0x80;
    if (p->reset) {
        p->reset = 0;
        CpuSet(p->source, p->palette, 0x04000008);
        p->state = 0;
        p->timer = 0;
        p->entered = 1;
    }
    switch (p->state) {
        case 0:
            Palette_InterpolateBanks(p->palette, p->source, p->target, phase, 7);
            if (phase & 0x80) {
                p->state = 1;
                p->timer = 0;
                p->entered = 1;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
