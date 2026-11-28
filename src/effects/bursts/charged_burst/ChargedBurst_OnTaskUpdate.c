#include "effects/bursts/charged_burst.h"

u32 ChargedBurst_OnTaskUpdate(struct ChargedBurst *burst)
{
    Callback_InvokeActor(burst, burst->state);
    return 0;
}
