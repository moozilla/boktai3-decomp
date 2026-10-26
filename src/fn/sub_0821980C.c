#include "global.h"
#include "render.h"

struct Obj {
    u8 pad[2];
    u16 f2;
    u8 pad4[0x17];
    u8 f1B;
};
s32 Anim_SelectWidgetGlyph(struct Obj *, u32, u32);

s32 Anim_LoadKeyframe(struct Obj *o, u32 b, u16 c, u8 d)
{
    if (Anim_SelectWidgetGlyph(o, b, c) < 0)
        return -1;
    o->f2 = gDefaultAnimFlags;
    o->f1B = d;
    return 0;
}
