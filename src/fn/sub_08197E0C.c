#include "global.h"

struct Fade08197E0C {
    u8 pad0[8];
    u32 colors[3];
    u8 from;
    u8 to;
    u8 state;
    u8 timer;
    u8 out[4];
};
u32 SunClock_GetPhase(void);
u8 TimePalette_GetSourceIndex(u32);
void Palette_InterpolateBanks(void *, u32, u32, u32, u32);

void TimePalette_UpdateTransition(struct Fade08197E0C *f)
{
    u32 state = SunClock_GetPhase();
    if (state != f->state) {
        f->from = TimePalette_GetSourceIndex(f->state);
        f->to = TimePalette_GetSourceIndex(state);
        f->state = state;
        f->timer = 0;
    }
    if (f->from != f->to) {
        u32 a, b;
        f->timer++;
        a = f->colors[f->from];
        b = f->colors[f->to];
        Palette_InterpolateBanks(f->out, a, b, f->timer, 6);
        if (f->timer > 0x3f)
            f->from = f->to;
    }
}
