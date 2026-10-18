#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_ReadPageVisibility sub_08177C9C
void sub_08177C9C(struct SceneHudState *p)
{
    s32 i;
    if (Script_SeekToKeyword('o')) {
        for (i = 0; i < 10; i++)
            p->pageEnabled[i] = Script_GetValue();
    } else {
        u32 one = 1;
        u8 *entry;
        i = 9;
        entry = &p->pageEnabled[9];
        do {
            *entry-- = one;
        } while (--i >= 0);
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
