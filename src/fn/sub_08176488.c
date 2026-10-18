#define SceneDialog_FinishFailedResult sub_08176488
#define SceneDialog_SetPhase Hud_SetInteractionCallback
#define SceneDialog_SelectMessage      sub_08175854
#include "global.h"
struct SceneDialog {
    u8 pad0[0x5c];
    const u8 *script;
    u8 pad60[0xA4E - 0x60];
    u16 timer;
    u8 padA50[0x48C8 - 0xA50];
    s32 window;
};
void Sound_PlaySong(u32);
void TextWindow_Clear(u32);
void sub_08175854(u32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08176224(void);
void sub_08176488(struct SceneDialog *p)
{
    if (!p->timer) {
        Sound_PlaySong(0x192);
        TextWindow_Clear(p->window);
        sub_08175854(15);
    }
    if (++p->timer > 119) {
        sub_08175854(8);
        sub_081660D0(p, 1, -1);
        Hud_SetInteractionCallback(p, sub_08176224, 0);
    }
}
