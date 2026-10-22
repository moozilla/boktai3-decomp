#include "global.h"

void Mimic_ResetExpiredTarget(void);

s32 Mimic_UpdateReactionState(void)
{
    Mimic_ResetExpiredTarget();
    return 1;
}
