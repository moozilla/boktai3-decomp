#include "global.h"
void Vorn_UpdateRecoveryCooldown(u8 *);
u32 Vorn_UpdateActionTimers(u8 *p)
{
    u8 *q = p;
    u16 *r;
    Vorn_UpdateRecoveryCooldown(p);
    switch (p[0x2B5]) {
        case 12:
        case 13:
        case 34:
        case 35:
        case 37:
        case 41:
        case 42: {
            u32 m = 4;
            u32 *x = (u32 *)(q + 0x2D0);
            *x = *x | m;
        } break;
    }
    r = (u16 *)(p + 0x69E);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6AC);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6D0);
    if (*r != 0)
        *r = *r - 1;
    return 1;
}
