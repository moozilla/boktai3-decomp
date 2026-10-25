#include "global.h"
#include "scene_hud_display.h"

void Presentation_DrawEntryCount(struct SceneHudState *);

void Presentation_SetAmountGlyphRows(struct SceneHudState *s)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        s->motionWidgets[2 + i * 3].flags &= ~1;
        s->motionWidgets[2 + i * 3].position.x = 40;
        s->motionWidgets[2 + i * 3].position.y = 104 + i * 8;
        s->motionWidgets[3 + i * 3].flags &= ~1;
        s->motionWidgets[3 + i * 3].position.x = 48;
        s->motionWidgets[3 + i * 3].position.y = 104 + i * 8;
        s->motionWidgets[4 + i * 3].flags &= ~1;
        s->motionWidgets[4 + i * 3].position.x = 56;
        s->motionWidgets[4 + i * 3].position.y = 104 + i * 8;
    }
    Presentation_DrawEntryCount(s);
}
