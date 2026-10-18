#include "global.h"
void Ghoul_TickCooldown(void);
u32 Ghoul_UpdateCooldown(void)
{
    Ghoul_TickCooldown();
    return 1;
}
