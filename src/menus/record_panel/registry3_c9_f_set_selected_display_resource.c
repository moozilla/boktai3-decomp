#include "engine/shared/global.h"

struct P {
    u8 f[0x42c];
    u16 v;
};
s32 SelectionPanel_GetSelection(u8 *);
s32 GameState_GetChallengeBestRank(u32);
void Anim_SelectWidgetGlyph(u8 *, u8 *, u32);
void Registry3C9F_SetSelectedDisplayResource(struct P *p)
{
    p->v = GameState_GetChallengeBestRank(SelectionPanel_GetSelection((u8 *)p)) + 0x22;
    Anim_SelectWidgetGlyph((u8 *)p + 0x260, (u8 *)p + 0x400, p->v);
}
