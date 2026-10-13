#include "global.h"
struct Wave {
    u8 pad[24];
    u16 id, target, mode;
    u8 start, end;
    u16 step, phase, period, timer;
    u32 enabled;
    u16 bank;
    s16 samples[1][288];
};
extern struct Wave *gUnk_0200007C;
extern const s16 gUnk_086149C4[];
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int ScanlineWave_Initialize(struct Wave *p, int id)
{
    int amplitude;
    int i, j;
    int limit;
    int wrap;
    const s16 *table;
    int mask;
    gUnk_0200007C = p;
    p->id = id;
    p->target = sub_0821ABA8('t', 0);
    p->mode = sub_0821ABA8('m', 0);
    if (Script_SeekToKeyword('r')) {
        p->start = Script_GetValue();
        p->end = Script_GetValue();
    } else {
        p->start = 0;
        p->end = 160;
    }
    amplitude = sub_0821ABA8('s', 8);
    p->timer = 0;
    p->step = sub_0821ABA8('o', 2);
    p->phase = 0;
    p->period = sub_0821ABA8('i', 5);
    p->bank = 0;
    p->enabled = 1;
    i = 0;
    limit = 287;
    wrap = 127;
    table = gUnk_086149C4;
    mask = 255;
    do {
        j = 0;
        do {
            int v = (table[(j & wrap) & mask] * amplitude) >> 12;
            p->samples[i][j] = v;
            j++;
        } while (j <= limit);
        i++;
    } while (i <= 0);
}
