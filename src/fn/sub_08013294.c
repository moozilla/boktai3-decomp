#include "breakable_barrier.h"

int BreakableBarrier_Initialize(struct BreakableBarrierManager *, struct BreakableBarrier *, unsigned);

int BreakableBarriers_Initialize(struct BreakableBarrierManager *m)
{
    int i;
    struct BreakableBarrier *p;
    m->count = 0;
    m->remaining = 0;
    m->health = Script_GetKeywordValue('l', 5);
    if (Script_SeekToKeyword('p')) {
        for (i = 0; i < 4 && Script_GetPc(); i++) {
            m->positions[i].x = Script_GetValue();
            m->positions[i].y = Script_GetValue();
            m->positions[i].z = Script_GetValue();
        }
        m->count = i;
        m->remaining = i;
    }
    if (Script_SeekToKeyword('d')) {
        u32 *d;
        i = 0;
        if (i < m->count) {
            d = m->direction;
            do {
                *d++ = Script_GetValue();
                i++;
            } while (i < m->count);
        }
    }
    p = m->barriers;
    for (i = 0; i < 4; i++, p++)
        if (BreakableBarrier_Initialize(m, p, i) < 0)
            return -1;
    m->script = Script_GetKeywordValue('c', 0);
    gUnk_03005214 = 0x1084;
    gUnk_030051C4 = 0x1FFF;
    {
        int v = 64 - (int)(m->remaining * 40) / m->count;
        if (v > 63)
            v = 64;
        else if (v < 0)
            v = 0;
        m->blend = v;
    }
    return 0;
}
