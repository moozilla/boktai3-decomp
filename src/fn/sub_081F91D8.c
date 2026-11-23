#include "global.h"
#include "ratatosk.h"
void Ratatosk_UpdateFlagWaitState18(struct Ratatosk *p)
{
    u32 m;
    u32 t;
    u8 *q = (u8 *)p;
    if (Ratatosk_ConsumeActionEntry(p)) {
        u32 v = 0x12;
        p->actionAnimation = v;
    }
    m = 1;
    t = p->flags & m;
    if (t == 0) {
        *(u32 *)(q + 0xF04) = m;
        p->actionMode = t;
        p->actionIndex = t;
        p->actionPhase = m;
        p->actionTimer = t;
        p->actionEnter = m;
    } else
        p->actionTimer++;
}
