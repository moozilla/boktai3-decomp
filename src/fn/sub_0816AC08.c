#include "global.h"
struct MenuPrompt {
    u8 unk0[0xC4];
    u8 animData[0x3C40 - 0xC4];
    u8 firstSprite[0x60];
    u8 secondSprite[0x80];
    s16 selectionX;
    s16 selectionY;
    u8 unk3D24[0x48E4 - 0x3D24];
    u8 selection;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void MenuPrompt_UpdateSelectionSprites(struct MenuPrompt *p)
{
    Anim_LoadKeyframe(p->firstSprite, p->animData, p->selection + 25, 1);
    Anim_LoadKeyframe(p->secondSprite, p->animData, (u16)(28 - p->selection), 1);
    p->selectionX = 0;
    p->selectionY = p->selection << 5;
}
