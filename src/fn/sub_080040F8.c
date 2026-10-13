#include "global.h"
#include "terrain_overlay.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 TerrainOverlay_Initialize(struct TerrainOverlay *, u32, u32);
void sub_0821A0C0(void *);
void TerrainOverlay_Update(void);
void TerrainOverlay_DestroyDrawables(void);

void *sub_080040F8(u16 x, u16 y)
{
    struct TerrainOverlay *s;
    s32 n;
    s32 a;
    s32 b;
    if (Script_SeekToKeyword(0x6e))
        n = Script_GetValue();
    else
        n = 8;
    a = n * 0x84;
    b = n * 8 + 0x90;
    s = sub_08219FBC(9, a + b + n * 12);
    if (s != 0) {
        sub_0821A04C(s, TerrainOverlay_Update, TerrainOverlay_DestroyDrawables);
        s->drawables = (struct TerrainOverlayDrawable *)((u8 *)s + b);
        s->keys = (u32 *)((u8 *)s->drawables + a);
        s->actorCount = n;
        s->drawableCount = n * 3;
        if (TerrainOverlay_Initialize(s, x, y) < 0) {
            sub_0821A0C0(s);
            return 0;
        }
    }
    return s;
}
