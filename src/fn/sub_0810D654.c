#include "global.h"

struct P {
    u8 f[0x42c];
    u16 v;
};
void SelectionPanel_GetSelection(u8 *);
s32 GameState_GetChallengeBestRank(void);
void Anim_SelectWidgetGlyph(u8 *, u8 *, u32);
void Registry3C9F_SetSelectedDisplayResource(struct P *p)
{
    SelectionPanel_GetSelection((u8 *)p);
    p->v = GameState_GetChallengeBestRank() + 0x22;
    Anim_SelectWidgetGlyph((u8 *)p + 0x260, (u8 *)p + 0x400, p->v);
}
