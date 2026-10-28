#include "global.h"
#include "render.h"
struct Wave {
    u8 pad[24];
    u16 id, target, mode;
    u8 start, end;
    u16 step, phase, period, timer;
    u32 enabled;
    u16 bank;
    s16 samples[1][288];
};
void ScanlineEffect_SetSource(int, int, void *);
int ScanlineWave_Update(struct Wave *p)
{
    if (p->enabled) {
        ScanlineEffect_SetSource(
            p->target, p->mode,
            &p->samples[p->bank][(gBgRecords[gScanlineEffectLayer].y + p->phase) & 127]);
        if (++p->timer >= p->period) {
            p->timer = 0;
            p->phase += p->step;
        }
    }
    return 0;
}
