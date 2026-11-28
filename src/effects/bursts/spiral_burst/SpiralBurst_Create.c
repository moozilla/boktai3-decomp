#include "effects/bursts/spiral_burst.h"

u32 SpiralBurst_OnTaskUpdate(struct SpiralBurst *);
u32 SpiralBurst_OnTaskDestroy(struct SpiralBurst *);
s32 SpiralBurst_Init(struct SpiralBurst *, void *, struct ContactPosition *);

struct SpiralBurst *SpiralBurst_Create(void *source, struct ContactPosition *origin)
{
    struct SpiralBurst *burst = Task_Create(8, sizeof(*burst));

    if (burst) {
        Task_SetCallbacks(burst, SpiralBurst_OnTaskUpdate, SpiralBurst_OnTaskDestroy);
        if (SpiralBurst_Init(burst, source, origin) < 0) {
            Task_Kill(burst);
            return 0;
        }
    }
    return burst;
}
