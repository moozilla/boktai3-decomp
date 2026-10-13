#include "global.h"
struct View {
    u8 pad[34];
    s16 y;
    u8 tail[12];
};
extern struct View gUnk_03004C30[];
extern int gUnk_03005220;
struct Wave {
    u8 pad[24];
    u16 id, target, mode;
    u8 start, end;
    u16 step, phase, period, timer;
    u32 enabled;
    u16 bank;
    s16 samples[1][288];
};
void sub_08219254(int, int, void *);
int ScanlineWave_Update(struct Wave *p)
{
    if (p->enabled) {
        sub_08219254(p->target, p->mode,
                     &p->samples[p->bank][(gUnk_03004C30[gUnk_03005220].y + p->phase) & 127]);
        if (++p->timer >= p->period) {
            p->timer = 0;
            p->phase += p->step;
        }
    }
    return 0;
}
