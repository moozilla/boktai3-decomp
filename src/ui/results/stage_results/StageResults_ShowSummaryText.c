#include "global.h"
struct S {
    u8 f[0x11C0];
    u32 a;
    u8 p[8];
    u32 b;
};
extern u8 *gGameState;
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_SetString(u32, u32, void *);
void TextWindow_StartPlayback(u32);
void StageResults_ShowSummaryText(struct S *s)
{
    u8 *q = gGameState + 0x790;
    u32 *p = &s->b;
    TextWindow_SetScript(*p, s->a);
    TextWindow_Refresh(*p, 0);
    TextWindow_SetString(*p, 1, q);
    TextWindow_StartPlayback(*p);
}
