#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 93.6%
// Open: the ROM rebuilds 0x2FC from scratch where this draft derives it from
// 0x300 - 4, and the first branch stores the position through r1.
struct ChallengePosition {
    s16 x, y;
};
struct ChallengeWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y;
};
void Palette_UpdateHighlightPulse(u16 *, u8 *);
s32 GameState_GetPackedTripletValue(u32, s32);
s32 ChallengeMenu_GetRankKeyframe(s32);

s32 sub_081C58BC(u8 *p)
{
    s32 *selection = (s32 *)(p + 0x300);
    s32 rank;
    if (*selection == 0) {
        Anim_LoadKeyframe(p + 0xd8, p + 0x38, 0x55, 0);
        ((struct ChallengePosition *)(p + 0xf8))->x = 0;
        ((struct ChallengePosition *)(p + 0xf8))->y = 0;
    } else if (*selection <= 3) {
        Anim_LoadKeyframe(p + 0xd8, p + 0x38, 0x56, 0);
        ((struct ChallengePosition *)(p + 0xf8))->x = 0;
        ((struct ChallengePosition *)(p + 0xf8))->y = (*selection - 1) << 4;
    }
    Palette_UpdateHighlightPulse((u16 *)(p + 0x2d8), p + 0x2e0);
    ((struct ChallengeWidget *)(p + 0x198))->flags &= ~1;
    if (*(s32 *)(p + 0x300) != 0) {
        if (*(s32 *)(p + 0x300) > 3)
            return;
        rank = ChallengeMenu_GetRankKeyframe(
            GameState_GetPackedTripletValue(*(u32 *)(p + 0x2fc), *(s32 *)(p + 0x300) - 1));
        if (rank >= 0) {
            Anim_LoadKeyframe(p + 0x138, p + 0x38, (u16)rank, 0);
            ((struct ChallengeWidget *)(p + 0x138))->flags &= ~1;
            return;
        }
    }
    ((struct ChallengeWidget *)(p + 0x138))->flags |= 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C58BC);
#endif
