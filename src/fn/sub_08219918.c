#include "global.h"
#include "render.h"

struct ClearedWidget {
    u16 f0;
    u8 pad2[2];
    u8 f4;
    u8 pad5;
    u16 f6;
    u8 pad8[0xA];
    u16 f12, f14, f16, f18;
    u8 pad1A[3];
    u8 f1D;
    u8 pad1E[0xA];
    u16 f28, f2A, f2C, f2E, f30, f32;
    u8 f34, f35;
    u16 f36;
    u8 pad38[4];
    u32 f3C, f40, f44, f48, f4C, f50, f54, f58, f5C;
};

s32 Anim_InitPositionedWidget(void *, void *, u32, u32, u32, u32, u32, void *);

s32 Widget_InitCleared(struct ClearedWidget *w, void *resource, u16 glyph, u32 flags, u8 a, u8 b,
                       u8 c, void *position)
{
    u32 zero = w->f4;
    if (zero != 0)
        return -1;
    else {
        w->f0 = zero;
        w->f6 = zero;
        w->f12 = zero;
        w->f14 = zero;
        w->f16 = zero;
        w->f18 = zero;
        w->f1D = 0;
        w->f28 = zero;
        w->f2A = zero;
        w->f2C = zero;
        w->f2E = zero;
        w->f30 = zero;
        w->f32 = zero;
        w->f34 = 0;
        w->f35 = 0;
        w->f36 = zero;
        w->f3C = zero;
        w->f54 = zero;
        w->f48 = zero;
        w->f4C = zero;
        w->f50 = zero;
        Anim_InitPositionedWidget(w, resource, glyph, flags, a, b, c, position);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        w->f40 = -1;
        w->f44 = -1;
        w->f58 = 0;
        w->f5C = 0;
        return 0;
    }
}
