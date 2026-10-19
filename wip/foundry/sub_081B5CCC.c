#define ScenePaletteActor_UpdateFadeOut sub_081B5CCC
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
    void *sourceEnd;
    void *target;
};
void sub_081B019C(void *, struct ScenePaletteActor *);
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void sub_081B5CCC(void *owner, struct ScenePaletteActor *p, s32 phase)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        CpuSet(p->source, p->palette, 0x04000008);
        p->state = 0;
        p->timer = 0;
        p->entered = 1;
    }
    switch (p->state) {
        case 0:
            Palette_InterpolateBanks(p->palette, p->source, p->sourceEnd, phase, 6);
            if (phase > 0x3F) {
                p->state = 1;
                p->timer = 0;
                p->entered = 1;
            }
            break;
        case 1:
            p->finished = 1;
            sub_081B019C(owner, p);
            break;
    }
}
