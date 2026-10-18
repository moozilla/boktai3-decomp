#define ResultPanel_DrawCappedSmallCounters sub_081641C4
#include "global.h"
struct ResultCounters { u8 pad00[0x424]; u16 first; u16 second; u16 smallFirst; u16 smallSecond; };
struct ResultPanel { u8 pad00[0xA40]; struct ResultCounters *counters; };
void sub_08163FFC(struct ResultPanel *, u32, u32, u32, u32, u32, u32, u32);
void sub_081641C4(struct ResultPanel *p, u32 a, u32 b, u32 c)
{
    u32 first = p->counters->smallFirst <= 999 ? p->counters->smallFirst : 999;
    u32 second = p->counters->smallSecond <= 999 ? p->counters->smallSecond : 999;
    sub_08163FFC(p, a, 100, 3, first, second, b, c);
}
