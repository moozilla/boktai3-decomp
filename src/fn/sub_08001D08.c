#include "global.h"
#include "background_controller.h"

static inline s32 next(s32 fallback)
{
    u8 *p = Script_GetPc();
    if (p)
        return sub_0821AA00(p);
    return fallback;
}

static inline s32 nextZero(void)
{
    s32 v = (s32)Script_GetPc();
    if (v)
        v = sub_0821AA00((u8 *)v);
    return v;
}

void *Background_CreateFromScript(u32 id)
{
    struct BackgroundConfig c;
    c.flags = sub_0821ABA8('f', 0);
    c.layer = sub_0821ABA8('i', 0);
    c.resource = sub_0821ABA8('d', 0);
    if (Script_SeekToKeyword('r')) {
        c.rateX = Script_GetValue();
        c.rateY = Script_GetValue();
    } else {
        c.rateX = 128;
        c.rateY = 128;
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
    c.priority = sub_0821ABA8('a', 0);
    if (Script_SeekToKeyword('s')) {
        c.x = nextZero();
        c.y = nextZero();
    } else {
        c.x = 0;
        c.y = 0;
    }
    if (Script_SeekToKeyword('m')) {
        c.scrollX = nextZero();
        c.scrollY = nextZero();
    } else {
        c.scrollX = 0;
        c.scrollY = 0;
    }
    if (Script_SeekToKeyword('l')) {
        c.limitX = nextZero();
        c.limitY = nextZero();
    } else {
        c.limitX = 0;
        c.limitY = 0;
    }
    if (Script_SeekToKeyword('c')) {
        c.centerX = next(120);
        c.centerY = next(80);
    } else {
        c.centerX = 120;
        c.centerY = 80;
    }
    if (Script_SeekToKeyword('A')) {
        c.offsetX = nextZero();
        c.offsetY = nextZero();
    } else {
        c.offsetX = 0;
        c.offsetY = 0;
    }
    if (Script_SeekToKeyword('b')) {
        s32 x = next(0), y = next(8);
        c.blendX = x;
        c.blendY = y;
        sub_08219574(c.layer, x, y);
    }
    return sub_08001CB0(id, &c);
}
