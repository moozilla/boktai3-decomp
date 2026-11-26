#include "global.h"
void CannonBattery_InitMuzzleFlash(void *, void *, s32, s32);
void CannonBattery_SpawnMuzzleFlash(u8 *p, s32 a, s32 b)
{
    u8 *q = 0;
    s32 i = 0;
    while (i <= 7) {
        s32 flags = *(u16 *)(p + 0x1e);
        if (!((flags >> i) & 1)) {
            q = p + 0xd68 + i * 0xb8;
            *(u16 *)(p + 0x1e) = flags | (1 << i);
            break;
        }
        i++;
    }
    if (q)
        CannonBattery_InitMuzzleFlash(p, q, a, b);
}
