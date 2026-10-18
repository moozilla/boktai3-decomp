#define SceneMenuPanel_UpdateMotion PauseMenu_UpdateSpriteMotion
#define SceneMenuPanel_SetCallback  Hud_SetInteractionCallback
#include "global.h"
struct SceneMenuPanel {
    u8 unknown0[0xA4E];
    u16 delay;
    u8 unknownA50[0x2058];
    u32 widgetFlags;
    u8 unknown2AAC[0x18DE];
    u8 selection;
};
u32 SceneMenuPanel_UpdateMotion(struct SceneMenuPanel *, u32);
void sub_0816D634(struct SceneMenuPanel *, u32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneMenuPanel *, void (*)(void), s32);
void SceneHud_ShowActionResultText(void);
void SceneMenuPanel_AdvanceAfterMotion(struct SceneMenuPanel *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->delay > 7) {
            Sound_PlaySong(269);
            panel->widgetFlags |= 1;
            sub_0816D634(panel, panel->selection);
            SceneMenuPanel_SetCallback(panel, SceneHud_ShowActionResultText, 1);
        }
    }
}
