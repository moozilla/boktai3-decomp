#include "global.h"
struct BoundsHalf {
    u32 lo : 16;
    s32 hi : 16;
};
struct SpriteBounds {
    struct BoundsHalf a, b;
};
void Position_GetHeightAdjustedCoordinates(void *, u32, void *);
void SpatialIndex_InitEntry(void *, void *, struct SpriteBounds *, struct SpriteBounds *, u32, u32,
                            u32, u32, u32, u32, u32);
void SceneProcParticles_SubmitBounds(u8 *p, u32 y, u32 x, u32 mode)
{
    u32 pos[2];
    struct SpriteBounds bounds, offset;
    Position_GetHeightAdjustedCoordinates(p + 0xa0, *(u32 *)(p + 0xc0), pos);
    bounds.a.lo = 12;
    bounds.a.hi = 8;
    bounds.b.lo = 8;
    *(u32 *)&offset.a = 0;
    *(u32 *)&offset.b &= 0xFFFF0000;
    SpatialIndex_InitEntry(p + 4, pos, &bounds, &offset, 12, mode, 5, x, 12, y, (u32)p);
}
