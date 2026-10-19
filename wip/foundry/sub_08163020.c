#include "global.h"
#define TransitionPanel_ApplyScaleAndSpacing sub_08163020
struct TransitionPanelSprite {
    u32 flags;
    u8 unknown4[4], scaleX, scaleY, unknownA[0x12];
    u16 x;
    u8 unknown1E[0x2A];
    s16 baseX;
    u8 unknown4A[6];
};
struct TransitionPanel {
    u8 task[0x18];
    struct TransitionPanelSprite sprites[8];
    u8 unknown298[0xA4];
    s16 extentA, extentB;
    u8 unknown340, state;
    u8 unknown342[4];
    u16 timer;
};
void sub_08163020(struct TransitionPanel *p)
{
    s32 offset;
    s16 extent;
    struct TransitionPanelSprite *sprite;
    s32 i;
    s16 *extentA = &p->extentA;
    s16 *extentB = &p->extentB;
    u8 *scale = &p->sprites[0].scaleX;
    for (i = 7; i >= 0; i--) {
        scale[0] = *extentA;
        scale[1] = *extentB;
        scale += 0x50;
    }
    i = 3;
    offset = 0;
    sprite = &p->sprites[3];
    do {
        if (p->extentA > 64) {
            sprite->x = sprite->baseX - offset;
            offset -= 16;
            extent = p->extentA;
            offset += extent >> 2;
        } else
            sprite->x = sprite->baseX;
        sprite--;
    } while (--i >= 0);
    offset = 0;
    sprite = &p->sprites[4];
    i = 3;
    do {
        if (p->extentA > 64) {
            sprite->x = sprite->baseX + offset;
            offset -= 16;
            extent = p->extentA;
            offset += extent >> 2;
        } else
            sprite->x = sprite->baseX;
        sprite++;
    } while (--i >= 0);
}
