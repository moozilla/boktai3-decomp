#include "global.h"
#include "scene_hud_display.h"
#include "engine.h"
void Sound_PlaySong(u32);
void SceneHud_PlayActionChangeCue(struct SceneHudState *p, u32 group, u32 mode, u32 selection)
{
    if (gEngineFlags & 0x1000) {
        if (group == 0 && mode == 0 && selection - 1 <= 2 && selection != p->previousCueSelection)
            Sound_PlaySong(0x21C);
    } else {
        if (p->actionCueInitialized == 0) {
            if (p->actionCueSuppressed == 0)
                Sound_PlaySong(0x21C);
            p->actionCueInitialized = 1;
        } else if (group == 0) {
            if (mode == 0 || (mode == 1 && selection == 0)) {
                if (selection != p->previousCueSelection)
                    Sound_PlaySong(0x21C);
            }
        }
    }
    p->previousCueGroup = group;
    p->previousCueSelection = selection;
}
