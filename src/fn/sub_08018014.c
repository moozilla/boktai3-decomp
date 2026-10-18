#include "global.h"

extern u8 *gUnk_02000090;
void Weather_ClearEffects(void);

u32 Weather_Destroy(void)
{
    Weather_ClearEffects();
    gUnk_02000090 = 0;
    return 0;
}
