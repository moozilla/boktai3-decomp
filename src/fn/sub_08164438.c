#include "global.h"

struct S {
    u8 f00[0x424];
    u16 h24;
    u16 h26;
};
struct P {
    u8 f00[0xA40];
    struct S *s;
};

void Hud_DrawHealthMeterCells(struct P *, u32, u32, u32, u32, u32, u32);

void Hud_DrawHealthMeter(struct P *p, u32 a, u32 b, u32 c, u32 d)
{
    Hud_DrawHealthMeterCells(p, a, b, c, d, p->s->h24, p->s->h26);
}
