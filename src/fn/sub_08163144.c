#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.2%
// foundry draft (worker best 93.2%); what differs is mostly register allocation and instruction order
#define TransitionPanel_InitSprites sub_08163144
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
void sub_08163144(struct TransitionPanel *p)
{
    s32 i;
    s32 x;
    struct TransitionPanelSprite *sprite;
    p->extentA = 1;
    p->extentB = 1;
    i = 0;
    sprite = p->sprites;
    {
        u8 *animation = sprite->animation;
        struct TransitionPanelSprite *drawable = sprite;
        x = 56;
        do {
            sprite->baseX = x;
            sprite->baseY = 72;
            if (i > 3)
                sprite->baseX += 16;
            AnimData_Load(animation, 0x654B);
            Sprite_InitUnlinked(drawable, animation, 0);
            sprite->priority = 0;
            sprite->flags = 0x1033;
            sprite->x = sprite->baseX;
            sprite->y = sprite->baseY;
            sprite->scaleX = p->extentA;
            sprite->scaleY = p->extentB;
            sprite->frame = i;
            sprite++;
            animation += 0x50;
            drawable++;
            x += 16;
            i++;
        } while (i < 8);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08163144);
#endif
