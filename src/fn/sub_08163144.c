#include "global.h"
struct TransitionPanelSprite {
    u32 flags;
    u8 unknown4[3], priority, scaleX, scaleY, unknownA[6];
    u16 frame;
    u8 unknown12[10];
    s16 x, y;
    u8 unknown20[12], animation[0x1C];
    s16 baseX, baseY;
    u8 unknown4C[4];
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
s32 AnimData_Load(void *, u32);
void Sprite_InitUnlinked(void *, void *, s32);
void TransitionPanel_InitSprites(struct TransitionPanel *p)
{
    s32 i;
    struct TransitionPanelSprite *sprite;
    p->extentA = 1;
    p->extentB = 1;
    for (i = 0; i < 8; i++) {
        s32 x = i * 16 + 56;
        sprite = &p->sprites[i];
        sprite->baseX = x;
        sprite->baseY = 72;
        if (i > 3)
            sprite->baseX += 16;
        AnimData_Load(sprite->animation, 0x654B);
        Sprite_InitUnlinked(sprite, sprite->animation, 0);
        sprite->priority = 0;
        sprite->flags = 0x1033;
        sprite->x = sprite->baseX;
        sprite->y = sprite->baseY;
        sprite->scaleX = p->extentA;
        sprite->scaleY = p->extentB;
        sprite->frame = i;
    }
}
