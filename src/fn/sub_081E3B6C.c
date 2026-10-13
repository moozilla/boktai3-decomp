#include "global.h"
// Provisional name: Actor_ProjectAndClampToScreen
struct Vec {
    s16 x, y, z;
};
struct Actor {
    u8 pad[0xddc];
    u32 flags;
    u8 gap[0xdf8 - 0xde0];
    struct Vec pos;
};
extern const u16 gUnk_03005418[];
static inline s32 project(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
static inline void screen(struct Vec *dst, struct Vec *src)
{
    s32 depth, height, y, z;
    s32 sx = src->x, sz = src->z;
    dst->x = project(sx - sz);
    depth = project(sx + sz);
    height = project(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gUnk_03005418[0] + 120;
    dst->y = y - gUnk_03005418[1] + 80;
    dst->z = z - gUnk_03005418[2];
}
// Project the actor into screen space and clamp it to the visible display.
void Actor_ProjectAndClampToScreen(struct Actor *p)
{
    struct Vec pos;
    p->flags |= 0x30;
    screen(&pos, &p->pos);
    if (pos.x > 240)
        pos.x = 240;
    else if (pos.x < 0)
        pos.x = 0;
    if (pos.y > 160)
        pos.y = 160;
    else if (pos.y < 0)
        pos.y = 0;
    p->pos.x = pos.x;
    p->pos.y = pos.y;
    p->pos.z = 0;
}
