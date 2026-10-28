#include "global.h"
struct Clock {
    u8 pad[24];
    s16 hour, minute, second, tick;
    int seconds, window;
    void *script;
    u8 armed[4];
};
extern u8 gSunClock[];
int SunClock_GetBaseHour(void);
int SunClock_GetBaseMinute(void);
int SunClock_GetSecond(void);
void ClockEvents_RunScript(void *, int);
static inline int distance(int a, int b)
{
    int d = a - b;
    if (a < b)
        d = b - a;
    if (d > 43199)
        d -= 86400;
    {
        int v = d;
        if (v < 0)
            v = -v;
        return v;
    }
}
static inline int scheduled(int index)
{
    int h = gSunClock[index] * 3600;
    return h + gSunClock[index + 1] * 60;
}
int ClockEvents_Update(struct Clock *p)
{
    int zero;
    int tick = (u16)p->tick + 1;
    do {
        zero = 0;
    } while (0);
    p->tick = tick;
    if (p->tick > 59) {
        p->tick = zero;
    } else {
        p->hour = SunClock_GetBaseHour();
        p->minute = SunClock_GetBaseMinute();
        p->second = SunClock_GetSecond();
        p->seconds = p->hour * 3600 + p->minute * 60 + p->second;
        if (distance(p->seconds, 43230) <= 30) {
            if (p->armed[0]) {
                p->armed[0] = zero;
                ClockEvents_RunScript(p, 0);
            }
        } else {
            p->armed[0] = 1;
        }
        {
            int when = scheduled(10), limit = p->window;
            if (distance(p->seconds, when) <= limit) {
                if (p->armed[1]) {
                    p->armed[1] = 0;
                    ClockEvents_RunScript(p, 1);
                }
            } else {
                p->armed[1] = 1;
            }
        }
        {
            int when = scheduled(12), limit = p->window;
            if (distance(p->seconds, when) <= limit) {
                if (p->armed[2]) {
                    p->armed[2] = 0;
                    ClockEvents_RunScript(p, 2);
                }
            } else {
                p->armed[2] = 1;
            }
        }
    }
    return 0;
}
