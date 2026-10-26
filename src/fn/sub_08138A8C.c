#include "global.h"
#include "ram.h"
void TimedSpriteEffect_RequestStop(u32 i)
{
    u8 *b = gSpriteSlots;
    if (b != 0 && i <= 3) {
        u8 *e = b + i * 0x60;
        if (*(u16 *)(e + 0x76) != 0)
            *(u16 *)(e + 0x74) = 1;
    }
}
