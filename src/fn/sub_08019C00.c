#include "global.h"
#include "ram.h"
struct P {
    u32 a, b;
};
void SpritePool_SetPosition(u8 *p, struct P *q)
{
    if (gSpritePoolManager != 0)
        *(struct P *)(p + 0x28) = *q;
}
