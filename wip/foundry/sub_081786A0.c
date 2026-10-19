#define Hud_DrawPlayerKindTiles sub_081786A0
#define gAreaEffectLinkState gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
u16 *sub_08163F70(u32, u32, u32);
static inline u16 PlayerKindTile(u32 kind)
{
    if (kind == 0) return 0xE0C0;
    if (kind == 1) return 0xE0C3;
    if (kind == 2) return 0xE0C2;
    if (kind == 3) return 0xE0C1;
    return 0xE0C4;
}
void sub_081786A0(void)
{
    u8 *kinds;
    u16 *dst;
    s32 i;
    u32 tile;
    struct AreaEffectLinkState *state = gUnk_020004B4;
    if (state) {
        u32 unavailable = state->unavailable;
        kinds = (u8 *)state->playerKinds;
        if (unavailable) goto absent;
    } else {
absent:
        kinds = 0;
    }
    dst = sub_08163F70(0, 0x19, 2);
    i = 4;
    do {
        tile = PlayerKindTile(*kinds);
        *dst++ = tile;
        kinds++;
    } while (--i >= 0);
}
