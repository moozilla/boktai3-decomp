#include "global.h"
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void sub_08177DEC(u32);
/* Re-applies the current HUD mode (+0xA5E). */
void sub_081780B4(void)
{
    if (gUnk_02000214)
        sub_08177DEC(gUnk_02000214[0xA5E]);
}
