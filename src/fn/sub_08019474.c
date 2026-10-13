#include "global.h"
struct Clock {
    u8 pad[24];
    s16 hour, minute, second, tick;
    int seconds, window;
    void *script;
    u8 armed[4];
};
int sub_08228D88(void);
int sub_08228D94(void);
int sub_08228DA0(void);
int Script_GetKeywordValue(int, int);
int ClockEvents_Initialize(struct Clock *p)
{
    int i;
    u8 *q;
    p->hour = sub_08228D88();
    p->minute = sub_08228D94();
    p->second = sub_08228DA0();
    p->seconds = p->hour * 3600 + p->minute * 60 + p->second;
    p->window = Script_GetKeywordValue('d', 60);
    p->script = (void *)Script_GetKeywordValue('p', 0);
    {
        int one = 1;
        int count = 3;
        q = &p->armed[3];
        do {
            *q = one;
            q--;
            count--;
        } while (count >= 0);
    }
    p->tick = 0;
    return 0;
}
