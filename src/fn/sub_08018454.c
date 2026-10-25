#include "global.h"
struct PaletteCycleEntry {
    u16 *a, *b, *dest;
    u8 count, step, flags, reverse;
    u8 length[2], hold[2];
    u8 held;
};
s32 Div(s32, s32);
u32 PaletteCycle_UpdateEntry(u8 *unused, struct PaletteCycleEntry *e)
{
    u16 *from, *to;
    s32 length, amount, inverse, i;
    if (e->reverse == 0) {
        if (e->step >= e->length[0]) {
            if (e->held >= e->hold[0]) {
                if (!(e->flags & 2)) {
                    e->reverse = 1;
                    e->step = 0;
                    e->held = 0;
                    from = e->b;
                    to = e->a;
                    length = e->length[1];
                } else {
                    from = e->a;
                    to = e->b;
                    length = e->length[0];
                }
            } else {
                from = e->a;
                to = e->b;
                length = e->length[0];
                e->held++;
            }
        } else {
            from = e->a;
            to = e->b;
            length = e->length[0];
            e->step++;
        }
    } else {
        if (e->step >= e->length[1]) {
            if (e->held >= e->hold[1]) {
                if (!(e->flags & 4)) {
                    e->reverse = 0;
                    e->step = 0;
                    e->held = 0;
                    from = e->a;
                    to = e->b;
                    length = e->length[0];
                } else {
                    from = e->b;
                    to = e->a;
                    length = e->length[1];
                }
            } else {
                from = e->b;
                to = e->a;
                length = e->length[1];
                e->held++;
            }
        } else {
            from = e->b;
            to = e->a;
            length = e->length[1];
            e->step++;
        }
    }
    if (e->flags & 1) {
        if (length > 0)
            amount = Div(e->step << 5, length);
        else
            amount = 0;
        inverse = 32 - amount;
        for (i = 0; i < e->count; i++) {
            u32 x = from[i], y = to[i];
            u32 rb, g, c;
            rb = ((x & 0x7C1F) * amount + (y & 0x7C1F) * inverse) & 0xF83E0;
            g = ((x & 0x3E0) * amount + (y & 0x3E0) * inverse) & 0x7C00;
            c = (rb | g) >> 5;
            e->dest[i] = c;
        }
    } else {
        for (i = 0; i < e->count; i++)
            e->dest[i] = to[i];
    }
    return 0;
}
