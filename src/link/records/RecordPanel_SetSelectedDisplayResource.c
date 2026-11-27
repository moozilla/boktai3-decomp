#include "global.h"

struct P {
    u8 f[0x42c];
    u16 v;
};
s32 SelectionPanel_GetSelection(u8 *);
s32 GameState_GetChallengeBestRank(u32);
void AnimSprite_SelectGlyph(u8 *, u8 *, u32);
void RecordPanel_SetSelectedDisplayResource(struct P *p)
{
    p->v = GameState_GetChallengeBestRank(SelectionPanel_GetSelection((u8 *)p)) + 0x22;
    AnimSprite_SelectGlyph((u8 *)p + 0x260, (u8 *)p + 0x400, p->v);
}
