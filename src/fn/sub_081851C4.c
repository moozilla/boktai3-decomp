#include "global.h"

u32 DirectionalActor_TryBeginState(u8 *, u32);

u32 ActorController_StartAlternateTurn(u8 *p, u32 v)
{
    s32 d;
    *(p + 0xB3A) = v;
    d = (*(p + 0xB3A) - *(p + 0xB39)) & 0xff;
    if (d <= 0x80) {
        return DirectionalActor_TryBeginState(p, 0x1a);
    }
    return DirectionalActor_TryBeginState(p, 0x1b);
}
