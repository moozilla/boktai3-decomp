#include "effects/bursts/charged_burst.h"

u32 ChargedBurst_OnTaskDestroy(struct ChargedBurst *burst)
{
    s32 i;

    Effect_Remove(&burst->core);
    for (i = 0; i < 8; i++)
        Effect_Remove(&burst->sparks[i]);
    return 0;
}
