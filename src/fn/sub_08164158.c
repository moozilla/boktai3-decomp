#define ResultPanel_DrawCappedCounters sub_08164158
#include "global.h"
struct ResultCounters { u8 pad00[0x424]; u16 first; u16 second; u16 smallFirst; u16 smallSecond; };
struct ResultPanel { u8 pad00[0xA40]; struct ResultCounters *counters; };
void sub_08163FFC(struct ResultPanel *, u32, u32, u32, u32, u32, u32, u32);
void sub_08164158(struct ResultPanel *p, u32 a, u32 b, u32 c)
{
    u32 first = p->counters->first <= 9999 ? p->counters->first : 9999;
    u32 second = p->counters->second <= 9999 ? p->counters->second : 9999;
    sub_08163FFC(p, a, 1000, 4, first, second, b, c);
}
