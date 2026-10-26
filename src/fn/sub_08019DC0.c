#include "global.h"
#include "ram.h"
void Sprite_LinkToList(u8 *, int);
u32 SpritePool_Activate(u8 *p, u32 callback, u32 context)
{
    u8 *manager = gSpritePoolManager;
    u32 id;
    if (!manager || p[4])
        return 0;
    if (*(u32 *)(p + 12) & 0x80)
        Sprite_LinkToList(p + 12, 1);
    else
        Sprite_LinkToList(p + 12, 0);
    *(u32 *)(p + 0x68) = callback;
    *(u32 *)(p + 0xac) = context;
    *(u32 *)(p + 8) = 0;
    p[4] = 1;
    id = ++*(u32 *)(manager + 0xb18);
    id = (id << 28) | (u32)p;
    *(u32 *)p = id;
    return id;
}
