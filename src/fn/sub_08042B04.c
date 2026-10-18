#include "global.h"

void Collider_Remove(u8 *);
void Shadow_Destroy(u8 *);
u32 Sprite_Remove(u8 *);

u32 ContactActor_DestroySlot(u8 *unused, u8 *p)
{
    Collider_Remove(p + 0x80);
    Shadow_Destroy(p + 0x12c);
    return Sprite_Remove(p + 0xe4);
}
