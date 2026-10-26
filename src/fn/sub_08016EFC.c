#include "global.h"
#include "ram.h"
void Anim_Remove(u8 *);
u32 FloatingEffectManager_Destroy(u8 *p)
{
    s32 i;
    u8 *q = p + 0x60;
    for (i = 7; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x84;
    }
    {
        u32 z = 0;
        gLinkedWidgetSet = z;
    }
}
