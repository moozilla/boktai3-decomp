#include "clock_events.h"

void ClockEvents_RunScript(struct ClockEvents *clock, s32 event)
{
    struct ClockEventArguments args;
    args.count = 1;
    args.values = &event;
    if (clock->procedure)
        Script_Call(clock->procedure, &args);
}

static inline int ClockEvents_Distance(int seconds, int eventTime)
{
    int difference = seconds - eventTime;
    if (seconds < eventTime)
        difference = eventTime - seconds;
    if (difference > 43199)
        difference -= 86400;
    {
        int distance = difference;
        if (distance < 0)
            distance = -distance;
        return distance;
    }
}

int ClockEvents_Update(struct ClockEvents *clock)
{
    int zero;
    int frame = (u16)clock->frame + 1;
    do {
        zero = 0;
    } while (0);
    clock->frame = frame;
    if (clock->frame > 59) {
        clock->frame = zero;
    } else {
        clock->hour = SunClock_GetBaseHour();
        clock->minute = SunClock_GetBaseMinute();
        clock->second = SunClock_GetSecond();
        clock->seconds = clock->hour * 3600 + clock->minute * 60 + clock->second;
        if (ClockEvents_Distance(clock->seconds, 43230) <= 30) {
            if (clock->armed[0]) {
                clock->armed[0] = zero;
                ClockEvents_RunScript(clock, 0);
            }
        } else {
            clock->armed[0] = 1;
        }
        {
            int eventTime = gSunClock.hourA * 3600 + gSunClock.minuteA * 60, limit = clock->window;
            if (ClockEvents_Distance(clock->seconds, eventTime) <= limit) {
                if (clock->armed[1]) {
                    clock->armed[1] = 0;
                    ClockEvents_RunScript(clock, 1);
                }
            } else {
                clock->armed[1] = 1;
            }
        }
        {
            int eventTime = gSunClock.hourB * 3600 + gSunClock.minuteB * 60, limit = clock->window;
            if (ClockEvents_Distance(clock->seconds, eventTime) <= limit) {
                if (clock->armed[2]) {
                    clock->armed[2] = 0;
                    ClockEvents_RunScript(clock, 2);
                }
            } else {
                clock->armed[2] = 1;
            }
        }
    }
    return 0;
}

u32 ClockEvents_Destroy(void)
{
    return 0;
}

int ClockEvents_Initialize(struct ClockEvents *clock)
{
    u8 *armed;
    clock->hour = SunClock_GetBaseHour();
    clock->minute = SunClock_GetBaseMinute();
    clock->second = SunClock_GetSecond();
    clock->seconds = clock->hour * 3600 + clock->minute * 60 + clock->second;
    clock->window = Script_GetKeywordValue('d', 60);
    clock->procedure = Script_GetKeywordValue('p', 0);
    {
        int one = 1;
        int count = 3;
        armed = &clock->armed[3];
        do {
            *armed = one;
            armed--;
            count--;
        } while (count >= 0);
    }
    clock->frame = 0;
    return 0;
}

struct ClockEvents *ClockEvents_Create(void)
{
    struct ClockEvents *clock;
    clock = Task_Create(8, sizeof(struct ClockEvents));
    if (clock != 0) {
        Task_SetCallbacks(clock, (void (*)(void *))ClockEvents_Update,
                          (void (*)(void *))ClockEvents_Destroy);
        if (ClockEvents_Initialize(clock) < 0) {
            Task_Kill(clock);
            return 0;
        }
    }
    return clock;
}
