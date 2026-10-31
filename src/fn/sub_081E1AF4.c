#include "global.h"
// Provisional name: SoundTest_CountEntries
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x20 - 12];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Resource {
    u32 words[8];
};
struct Viewer {
    u8 p0[0x20];
    struct Sprite background;
    struct Resource backgroundResource;
    u8 p1[8];
    struct Sprite panel;
    struct Resource panelResource;
    u8 p2[8];
    struct Sprite cursor;
    struct Resource cursorResource;
    u8 p3[8];
    struct Sprite label;
    struct Resource labelResource;
    u8 p4[8];
    struct Resource digitsResource;
    struct Sprite digitsA[3], digitsB[3], labels[3];
    u8 p5[0x5cc - 0x5c0];
    u32 state, selection, countA, countB, countC, textIndex, valueA, valueB;
};
void Anim_LoadKeyframe(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
static inline s32 count(const s16 *list)
{
    const s16 *cursor = list;
    s32 count = 0;
    s32 entry = *cursor;
    while (entry != -1) {
        cursor++;
        count++;
        entry = *cursor;
    }
    return count;
}
// Count entries in each of the three sound-test categories.
void SoundTest_CountEntries(struct Viewer *p)
{
    p->countA = count(gUnk_086136F0);
    p->countB = count(gUnk_086137AE);
    p->countC = count(gUnk_08613BC4);
}
