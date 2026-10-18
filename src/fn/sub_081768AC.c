#define SceneDialog_UpdateEntryFade sub_081768AC
#define SceneDialog_SetCallback Hud_SetUpdateCallback
#include "global.h"
struct SceneDialogFade {
    u8 pad0[0xA4C];
    u16 timer;
};
void sub_0822B410(void);
void Sound_PlaySong(u32);
void Hud_InitSelectionBounds(u32, u32);
void sub_081765D4(void *);
void sub_08176928(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void sub_081768AC(struct SceneDialogFade *p)
{
    if (p->timer == 1)
        sub_0822B410();
    if (p->timer == 2)
        Sound_PlaySong(0x118);
    if (p->timer <= 15)
        Hud_InitSelectionBounds(64 - p->timer * 4, 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    if (++p->timer > 18) {
        sub_081765D4(p);
        Hud_SetUpdateCallback(p, sub_08176928);
    }
}
