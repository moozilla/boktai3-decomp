#include "global.h"

typedef struct Vec4 {
    s16 x, y, z, w;
} Vec4;
void Collider_Init(u8 *, s32, s32, s32, s32, Vec4 *, Vec4 *);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Player_ApplyContactCosts(void);

// confidence=low: configures the link player's attack collider for the current action.
void sub_0823A830(u8 *p)
{
    u8 *col = p + 0xb8c;
    Vec4 size;
    Vec4 vel;
    s32 power;
    if (p[0x418] == 6) {
        size.x = 0xaa;
        size.y = 0x32;
        size.z = 0xaa;
        vel.x = 0;
        vel.y = -20;
        vel.z = 0;
        Collider_Init(col, 0, 0x2101, 0, (u16)(1 << p[0x2c]), &size, &vel);
        Collider_SetAttackProperties(col, 0x14, 0x32, 0x10, 2, 0x1e);
        Collider_SetCallback(col, 0, p);
        return;
    }
    switch (p[0xbf5]) {
        case 0:
            size.x = 0x5a;
            size.y = 0x32;
            size.z = 0x5a;
            vel.x = 0;
            vel.y = -25;
            vel.z = 0;
            power = 1;
            *(u16 *)(p + 0xbfc) = 0x20;
            *(u16 *)(p + 0xbfe) = 0x14;
            break;
        case 1:
            size.x = 0x3c;
            size.y = 0x64;
            size.z = 0x3c;
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            power = 2;
            *(u16 *)(p + 0xbfc) = 0x40;
            *(u16 *)(p + 0xbfe) = 0x28;
            break;
        case 2:
            size.x = 0x50;
            size.y = 0x46;
            size.z = 0x50;
            vel.x = 0;
            vel.y = 0x1e;
            vel.z = 0;
            power = 4;
            *(u16 *)(p + 0xbfc) = 0x30;
            *(u16 *)(p + 0xbfe) = 0x28;
            break;
        case 4:
            size.x = 0x3c;
            size.y = 0x32;
            size.z = 0x3c;
            vel.x = 0;
            vel.y = -25;
            vel.z = 0;
            power = 0x10;
            *(u16 *)(p + 0xbfc) = 0x20;
            *(u16 *)(p + 0xbfe) = 0x1e;
            break;
        default:
            return;
    }
    Collider_Init(col, 0, 0x2101, 0, (u16)(1 << p[0x2c]), &size, &vel);
    Collider_SetAttackProperties(col, *(u16 *)(p + 0xbfa), *(u16 *)(p + 0xbfc), power, 0,
                                 *(u16 *)(p + 0xbfe));
    Collider_SetCallback(col, Player_ApplyContactCosts, p);
}
