#include "global.h"
#define PanelText_GetGlyphCell Video_GetWrappedBackgroundMapEntry
#define PanelText_Clear        Video_DrawBackgroundRowMarkers
struct LinkTimeRecord {
    u8 pad0[0x24];
    s32 times[1];
};
s32 Div(s32, s32);
u16 *Video_GetWrappedBackgroundMapEntry(s32, u8 *, u32);
void PanelText_DrawDigits(s32, s32, u8 *, u32, s32);
void Video_DrawBackgroundRowMarkers(u8 *, u32);
void LinkPanel_DrawTime(struct LinkTimeRecord *p, u8 *text, u32 style, s32 index)
{
    s32 value, whole, fraction;
    if (!p)
        Video_DrawBackgroundRowMarkers(text, style);
    value = p->times[index];
    if (value < 0) {
        Video_DrawBackgroundRowMarkers(text, style);
        return;
    }
    if (value > 59999) {
        fraction = 999;
        whole = fraction;
    } else {
        whole = Div(value, 60);
        fraction = Div((value - whole * 60) * 169, 10);
    }
    PanelText_DrawDigits(whole, 100, text, style, 0);
    PanelText_DrawDigits(fraction, 100, text + 4, style, 0);
    *Video_GetWrappedBackgroundMapEntry(3, text + 7, style) = 1;
}
