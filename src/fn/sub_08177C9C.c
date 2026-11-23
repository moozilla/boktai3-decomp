#include "global.h"
#include "scene_hud_display.h"
void SceneHud_ReadPageVisibility(struct SceneHudState *p)
{
    s32 i;
    if (Script_SeekToKeyword('o')) {
        for (i = 0; i < 10; i++)
            p->pageEnabled[i] = Script_GetValue();
    } else {
        for (i = 0; i < 10; i++)
            p->pageEnabled[i] = 1;
    }
    if (!p->pageEnabled[p->mode]) {
        for (i = 0; i < 10; i++) {
            if (++p->mode > 9)
                p->mode = 0;
            if (p->pageEnabled[p->mode])
                return;
        }
        p->mode = 0;
    }
}
