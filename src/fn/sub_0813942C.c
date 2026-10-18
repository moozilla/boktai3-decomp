#define SceneProcSprites_SubmitBounds sub_0813942C
#include "global.h"
struct BoundsHalf {
    u32 lo : 16;
    s32 hi : 16;
};
struct SpriteBounds {
    struct BoundsHalf a, b;
};
void Position_GetHeightAdjustedCoordinates(void *, u32, void *);
void sub_0811B278(void *, void *, struct SpriteBounds *, struct SpriteBounds *, u32, u32, u32, u32,
                  u32, u32, u32);
void sub_0813942C(u8 *p, u32 y, u32 x, u32 mode)
{
    u32 pos[2];
    struct SpriteBounds bounds, offset;
    Position_GetHeightAdjustedCoordinates(p + 0x9c, 0, pos);
    bounds.a.lo = 14;
    bounds.a.hi = 16;
    bounds.b.lo = 14;
    *(u32 *)&offset.a = -bounds.a.hi << 16;
    *(u32 *)&offset.b &= 0xFFFF0000;
    sub_0811B278(p + 0x5c, pos, &bounds, &offset, 12, mode, 5, x, 0x28, y, (u32)p);
}
