#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.8%
// foundry draft (worker best 91.8%); what differs is mostly register allocation and instruction order
#define TimerDisplay_DrawRecord sub_0810E9D4
#define TileNumber_GetCell Video_GetWrappedBackgroundMapEntry
#define TileNumber_DrawDecimal PanelText_DrawDigits
#define TimerDisplay_DrawEmpty Video_DrawBackgroundRowMarkers
struct TimerRecords { u8 unknown[0x24c]; s32 ticks[2]; };
s32 Div(s32, s32);
u32 Video_GetWrappedBackgroundMapEntry(s32, s32, u32);
void PanelText_DrawDigits(s32, s32, s32, u32, s32);
void Video_DrawBackgroundRowMarkers(s32, u32);
void sub_0810E9D4(struct TimerRecords *records, s32 x, u32 y, s32 unused, s32 index)
{
    s32 ticks, seconds, fraction;
    { u16 *cell = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x + 3, y); u16 value = 31; *cell = value; }
    if (!records) Video_DrawBackgroundRowMarkers(x, y);
    if (index < 0) index = 0;
    if (index > 1) index = 1;
    ticks = records->ticks[index];
    if (!ticks) { Video_DrawBackgroundRowMarkers(x, y); }
    else {
        if (ticks > 59999) { fraction = 999; seconds = fraction; }
        else { seconds = Div(ticks, 60); fraction = Div((ticks - seconds * 60) * 169, 10); }
        PanelText_DrawDigits(seconds, 100, x, y, 0);
        PanelText_DrawDigits(fraction, 100, x + 4, y, 0);
        { u16 *cell = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x + 7, y); u16 value = 1; *cell = value; }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810E9D4);
#endif
