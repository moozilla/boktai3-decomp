#include "global.h"
#include "ram.h"

u32 Task_Create(u32, u32);
void Task_SetCallbacks(u32, void (*)(void), void (*)(void));
s32 sub_081D33A0(u32, u32, u32);
void Task_Kill(u32);
void sub_081D31EC(void);
void PlayerParticleCloud_Destroy(void);

u32 PlayerParticleCloud_Create(u32 a, u32 b)
{
    u32 r;
    if (gCloudTask != 0)
        return gCloudTask;
    {
        r = Task_Create(0xa, 0xb74);
        if (r != 0) {
            Task_SetCallbacks(r, sub_081D31EC, PlayerParticleCloud_Destroy);
            if (sub_081D33A0(r, a, b) < 0) {
                Task_Kill(r);
                return 0;
            }
        }
    }
    return r;
}
