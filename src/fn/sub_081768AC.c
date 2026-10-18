#define SceneDialog_SetCallback Hud_SetUpdateCallback
#include "global.h"
struct SceneDialogFade {
    u8 pad0[0xA4C];
    u16 timer;
};
void MusicSlots_StopAllAndRetainResumable(void);
void Sound_PlaySong(u32);
void Hud_InitSelectionBounds(u32, u32);
void PauseMenu_SaveDisplayState(void *);
void Hud_UpdateBlendRamp(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void SceneDialog_UpdateEntryFade(struct SceneDialogFade *p)
{
    if (p->timer == 1)
        MusicSlots_StopAllAndRetainResumable();
    if (p->timer == 2)
        Sound_PlaySong(0x118);
    if (p->timer <= 15)
        Hud_InitSelectionBounds(64 - p->timer * 4, 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    if (++p->timer > 18) {
        PauseMenu_SaveDisplayState(p);
        Hud_SetUpdateCallback(p, Hud_UpdateBlendRamp);
    }
}
