#include "global.h"
#define ScrollingBackground_CreateFromScript sub_08002894
struct Config {
    u8 flags, layer;
    u16 resource;
    s32 x, y, offsetX, offsetY, speedX, speedY, scale, width, height, mode, priority, fade, blendX,
        blendY;
};
s32 sub_0821ABA8(s32, s32);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
s32 sub_0821AA00(u8 *);
void *sub_08002840(u16, struct Config *);
static inline s32 nextZero(void)
{
    s32 v = (s32)Script_GetPc();
    if (v)
        v = sub_0821AA00((u8 *)v);
    return v;
}
static inline s32 nextOffset(void)
{
    s32 v = (s32)Script_GetPc();
    if (v)
        v = sub_0821AA00((u8 *)v) << 4;
    return v;
}
static inline s32 next(s32 fallback)
{
    u8 *p = Script_GetPc();
    if (p)
        return sub_0821AA00(p);
    return fallback;
}
void *ScrollingBackground_CreateFromScript(u32 id)
{
    struct Config c;
    c.flags = sub_0821ABA8('f', 0);
    c.layer = sub_0821ABA8('i', 0);
    c.resource = sub_0821ABA8('d', 0);
    c.scale = sub_0821ABA8('r', 128);
    c.priority = sub_0821ABA8('a', 0);
    if (Script_SeekToKeyword('s')) {
        c.x = nextZero();
        c.y = nextZero();
    } else {
        c.x = 0;
        c.y = 0;
    }
    if (Script_SeekToKeyword('o')) {
        c.offsetX = nextOffset();
        c.offsetY = nextOffset();
    } else {
        c.offsetX = 0;
        c.offsetY = 0;
    }
    if (Script_SeekToKeyword('m')) {
        c.speedX = nextZero();
        c.speedY = nextZero();
    } else {
        c.speedX = 0;
        c.speedY = 0;
    }
    if (Script_SeekToKeyword('l')) {
        c.width = nextZero();
        c.height = nextZero();
    } else {
        c.width = 0;
        c.height = 0;
    }
    if (Script_SeekToKeyword('b')) {
        s32 x = next(0), y = next(8);
        c.blendX = x;
        c.blendY = y;
        c.fade = 1;
    } else {
        c.blendX = 16;
        c.blendY = 16;
        c.fade = 0;
    }
    switch (c.layer) {
        case 0:
            c.mode = sub_0821ABA8('p', 0);
            break;
        case 2:
            c.mode = sub_0821ABA8('p', 3);
            break;
        case 1:
        case 3:
            c.mode = sub_0821ABA8('p', 1);
            break;
    }
    return sub_08002840(id, &c);
}
