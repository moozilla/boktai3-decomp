#include "global.h"
#include "player.h"
void ResourceStation_InitializeSprite(u8 *);
void ResourceStation_SetTriggerPosition(u8 *);
void ResourceStation_InitializeCollider(u8 *);
void ResourceStation_InitializeDigits(u8 *);
s32 ResourceStation_Initialize(u8 *p, u32 v)
{
    u32 g = (*(u32 *)gPlayers);
    u32 z;
    u16 *h;
    *(u32 *)(p + 0x25c) = g;
    if (g == 0)
        return -1;
    h = (u16 *)(p + 0x260);
    z = 0;
    *h = v;
    ResourceStation_InitializeSprite(p);
    ResourceStation_SetTriggerPosition(p);
    ResourceStation_InitializeCollider(p);
    ResourceStation_InitializeDigits(p);
    p[0x262] = z;
    p[0x264] = z;
    p[0x265] = z;
    p[0x263] = z;
    if (*(u32 *)(p + 0x18) & 4)
        *(u16 *)(p + 0x266) = 7;
    else
        *(u16 *)(p + 0x266) = 1;
    return 0;
}
