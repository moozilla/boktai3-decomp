#include "global.h"
extern u32 gUnk_020000B0;
struct P {
    u32 a, b;
};
void SpritePool_SetPosition(u8 *p, struct P *q)
{
    if (gUnk_020000B0 != 0)
        *(struct P *)(p + 0x28) = *q;
}
