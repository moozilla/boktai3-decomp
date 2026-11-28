#include "effects/bursts/charged_burst.h"

void ChargedBurst_SetState(struct ChargedBurst *burst, void (*state)(struct ChargedBurst *))
{
    burst->state = state;
    burst->timer = 0;
}
