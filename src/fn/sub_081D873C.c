// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pada, rate, timer;
};
struct Actor {
    u32 tick, flags;
    u8 pad8[0x10];
    u16 cfgFlags, cfgDuration;
    u8 pad1c[0x8c - 0x1c];
    struct Render render;
    u8 pad9e[0x180 - 0xa0];
    struct Frames frames;
};
s32 Task4E01_StepTimedSequence(struct Actor *p)
{
    struct Render *r;
    struct Frames *f;
    struct Frames *initial;
    u16 *word;
    u16 *cfg = &p->cfgFlags;
    u32 two = 2;
    p->flags |= two;
    r = &p->render;
    initial = &p->frames;
    f = initial;
    word = f->data + f->index;
    r->tile = *word >> 6;
    {
        int settings = f->flags;
        int mask = 1;
        register int mine asm("r1");
        int bits;
        asm("" : "+r"(mask));
        mine = mask & settings;
        bits = (*word & 48) >> 4;
        bits &= mask;
        if (mine != bits)
            r->flags |= 4;
        else
            r->flags &= ~4;
    }
    {
        int settings = f->flags;
        int mask = 2;
        register int mine asm("r1");
        int bits;
        asm("" : "+r"(mask));
        mine = mask & settings;
        asm("" : "+r"(mine));
        mine = (u8)mine;
        mine = (u8)mine;
        bits = (*word & 48) >> 4;
        bits &= mask;
        if (mine != bits)
            r->flags |= 8;
        else
            r->flags &= ~8;
    }
    {
        u32 mask;
        int duration;
        register int savedDuration asm("r3");
        u32 timer = f->timer + 1;
        f->timer = timer;
        {
            register u32 initialMask asm("r1") = 65535;
            asm("" : "+r"(initialMask));
            mask = initialMask;
        }
        duration = f->duration;
        timer = (u16)timer;
        savedDuration = duration;
        if (timer >= (u32)duration) {
            register u32 flags asm("r1");
            int index;
            f->timer = 0;
            flags = f->flags;
            if (flags & 4) {
                index = f->index;
                if (!index) {
                    if (flags & 8) {
                        f->timer = savedDuration;
                        goto update;
                    }
                    index = f->count;
                }
                index--;
                goto storeIndex;
            } else {
                int count;
                index = f->index + 1;
                f->index = index;
                count = f->count;
                if ((index & mask) < count)
                    goto update;
                index = flags & 8;
                index = (u8)index;
                if (index) {
                    f->timer = duration;
                    index = count - 1;
                }
            }
        storeIndex:
            f->index = index;
        update:
            word = f->data + f->index;
            {
                u32 value = *word;
                u32 low = 15;
                low &= value;
                f->multiplier = low;
            }
            {
                s32 d = (f->multiplier * f->rate) >> 6;
                f->duration = d;
                if (!(d << 24))
                    f->duration = 1;
            }
        }
    }
    {
        register struct Actor *q asm("r2") = p;
        asm("" : "+r"(q));
        if (q->tick >= cfg[1])
            return 2;
    }
    return 0;
}
