#include "global.h"
struct PhaseObject {
    u8 pad0[0x4B8];
    s32 value;
    u8 pad4BC[0xB52 - 0x4BC];
    u16 limit;
    u16 step;
    u8 padB56[0xCDC - 0xB56];
    void (*next)(struct PhaseObject *);
    u8 padCE0[0xD80 - 0xCE0];
    u32 frame;
};
void PhaseObject_TickCountdown(struct PhaseObject *);

/* Every fourth frame, grow value by step until it reaches limit, then
   hand over to PhaseObject_TickCountdown. */
void sub_080F2BE4(struct PhaseObject *p)
{
    if (!(p->frame & 3)) {
        if (p->value >= p->limit)
            p->next = PhaseObject_TickCountdown;
        else
            p->value += p->step;
    }
}
