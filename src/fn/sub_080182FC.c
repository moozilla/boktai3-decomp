#include "global.h"
#include "ram.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08018028(u8 *);
void Task_Kill(u8 *);
void Weather_Update(void);
void Weather_Destroy(void);
u8 *Weather_Create(void)
{
    u8 *r;
    if (gWeatherState != 0)
        return gWeatherState;
    r = Task_Create(9, 0x8ec);
    if (r) {
        Task_SetCallbacks(r, Weather_Update, Weather_Destroy);
        if (sub_08018028(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
