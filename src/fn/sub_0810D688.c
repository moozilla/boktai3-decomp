#include "global.h"
#include "engine.h"
#include "ram.h"
struct RecordPanel {
    u8 pad0[0x268];
    u32 spriteFlags;
};
s32 SelectionPanel_GetSelection(struct RecordPanel *);
s32 GameState_GetChallengeBestScore(u32);
void Video_DrawBackgroundRowMarkers(u32, u32);
void Registry3C9F_SetSelectedDisplayResource(struct RecordPanel *);
s32 Div(s32, s32);
void PanelText_DrawDecimal(s32, s32, s32, s32, s32);
u16 *Bg_GetWrappedTilemapEntry(u32, u32, u32);
void RecordPanel_DrawTime(struct RecordPanel *p)
{
    s32 time;
    s32 index;
    s32 fraction;
    s32 seconds;
    u32 mask;
    index = SelectionPanel_GetSelection(p);
    time = GameState_GetChallengeBestScore(index);
    mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        index -= 6;
        if (time < 0)
            goto unavailable;
        if (index <= 4)
            goto available;
        goto unavailable;
    } else if (time < 0) {
    unavailable:
        Video_DrawBackgroundRowMarkers(2, 12);
        p->spriteFlags |= 1;
        return;
    }
available:
    Registry3C9F_SetSelectedDisplayResource(p);
    p->spriteFlags &= ~1;
    if (time > 59999) {
        seconds = 999;
        mask = seconds;
        fraction = mask;
    } else {
        seconds = Div(time, 60);
        fraction = Div(({
                           s32 t = time - seconds * 60;
                           t * 169;
                       }),
                       10);
    }
    {
        int zero = 0;
        PanelText_DrawDecimal(seconds, 100, 2, 12, zero);
        PanelText_DrawDecimal(fraction, 100, 6, 12, zero);
    }
    *Bg_GetWrappedTilemapEntry(3, 9, 12) = 1;
}
