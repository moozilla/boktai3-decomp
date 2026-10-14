#include "global.h"

struct Sprite08078B48 {
    u8 pad[0xf];
    u8 flags;
};
void sub_080789EC(u8 *);

static inline u8 TestMask(struct Sprite08078B48 *s, u32 mask)
{
    if (s->flags & mask)
        return TRUE;
    return FALSE;
}

static inline u32 OrV(u16 m, u16 *f)
{
    return m | *f;
}

u32 sub_08078B48(u8 *p)
{
    struct Sprite08078B48 *sprite;
    if (*(s32 *)(p + 0x304) > 0) {
        u8 m;
        sub_080789EC(p);
        {
            s32 v = OrV(0x80, (u16 *)(p + 0x2d6)) & ~0x100;
            *(u16 *)(p + 0x2d6) = v;
        }
        sprite = (void *)(p + 0xdc);
        m = 0x10;
        sprite->flags |= m;
        return 1;
    }
    sprite = (void *)(p + 0xdc);
    if (TestMask(sprite, 2)) {
        s32 m = ~0x10;
        sprite->flags &= m;
    }
    return 0;
}
