#include "effects/bursts/charged_burst.h"

u32 ChargedBurst_OnTaskUpdate(struct ChargedBurst *);
u32 ChargedBurst_OnTaskDestroy(struct ChargedBurst *);
s32 ChargedBurst_Init(struct ChargedBurst *, struct ContactPosition *, u32, u32, u32, u32);

struct ChargedBurst *ChargedBurst_Create(struct ContactPosition *position, u32 contactFlags,
                                         u32 power, u32 b, u32 d)
{
    struct ChargedBurst *burst = Task_Create(8, sizeof(struct ChargedBurst));

    if (burst) {
        Task_SetCallbacks(burst, ChargedBurst_OnTaskUpdate, ChargedBurst_OnTaskDestroy);
        if (ChargedBurst_Init(burst, position, contactFlags, power, b, d) < 0) {
            Task_Kill(burst);
            return 0;
        }
    }
    return burst;
}
