#include "global.h"
#include "engine/engine_pause.h"
#include "ui/widgets/selection_panel.h"
s32 GameState_GetChallengeBestScore(u32);
void Video_DrawBackgroundRowMarkers(u32, u32);
void RecordPanel_SetSelectedDisplayResource(struct SelectionPanel *);
s32 Div(s32, s32);
void PanelText_DrawDecimal(s32, s32, s32, s32, s32);
u16 *Bg_GetWrappedTilemapEntry(u32, u32, u32);

void RecordPanel_DrawTime(struct SelectionPanel *panel)
{
    s32 index = SelectionPanel_GetSelection(panel);
    s32 time = GameState_GetChallengeBestScore(index);
    s32 seconds, fraction;
    u32 value = 0x800;
    if (Engine_PauseBits(value)) {
        index -= 6;
        if (time < 0 || index > 4) {
            Video_DrawBackgroundRowMarkers(2, 12);
            panel->widgets[6].flags |= 1;
            return;
        }
    } else if (time < 0) {
        Video_DrawBackgroundRowMarkers(2, 12);
        panel->widgets[6].flags |= 1;
        return;
    }
    RecordPanel_SetSelectedDisplayResource(panel);
    panel->widgets[6].flags &= ~1;
    if (time > 59999) {
        seconds = 999;
        value = seconds;
        fraction = value;
    } else {
        seconds = Div(time, 60);
        fraction = Div((time - seconds * 60) * 169, 10);
    }
    PanelText_DrawDecimal(seconds, 100, 2, 12, 0);
    PanelText_DrawDecimal(fraction, 100, 6, 12, 0);
    *Bg_GetWrappedTilemapEntry(3, 9, 12) = 1;
}
