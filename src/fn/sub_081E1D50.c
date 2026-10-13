#include "global.h"
// Provisional name: SoundTest_PlaySelection
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
    s32 state, selection, countA, countB, countC, textIndex, valueA, valueB, previousA, previousB,
        previousC;
};
void Anim_LoadKeyframe(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
u32 sub_0822B590(s32);
void Sound_StopSong(s32);
void Sound_PlaySong(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
// Play the selected sound, stopping the previous sound in that category.
void SoundTest_PlaySelection(struct Viewer *p)
{
    switch (p->selection) {
        case 0:
            if (sub_0822B590(gUnk_086136F0[p->previousA]))
                Sound_StopSong(gUnk_086136F0[p->previousA]);
            Sound_PlaySong(gUnk_086136F0[p->textIndex]);
            p->previousA = p->textIndex;
            break;
        case 1:
            if (sub_0822B590(gUnk_086137AE[p->previousB]))
                Sound_StopSong(gUnk_086137AE[p->previousB]);
            Sound_PlaySong(gUnk_086137AE[p->valueA]);
            p->previousB = p->valueA;
            break;
        case 2:
            if (sub_0822B590(gUnk_08613BC4[p->previousC]))
                Sound_StopSong(gUnk_08613BC4[p->previousC]);
            Sound_PlaySong(gUnk_08613BC4[p->valueB]);
            p->previousC = p->valueB;
            break;
    }
    Anim_SetKeyframes(&p->labels[p->selection], &p->digitsResource, 0, 1, 0);
}
