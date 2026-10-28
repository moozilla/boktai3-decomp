#include "global.h"
#include "area_effect_link.h"
struct S {
    u8 filler[0x84];
    s16 a;
    s16 b;
};
void PanelText_DrawDigits(s32, s32, s32, s32, s32);
void Registry33FF_DrawActorValues(void)
{
    struct S *s = gLinkSession;
    if (s == 0) {
        PanelText_DrawDigits(0, 0x64, 0, 0x12, (s32)s);
        PanelText_DrawDigits(0, 0x64, 0x12, 0x12, (s32)s);
    } else {
        PanelText_DrawDigits(s->a, 0x64, 0, 0x12, 0);
        PanelText_DrawDigits(s->b, 0x64, 0x12, 0x12, 0);
    }
}
