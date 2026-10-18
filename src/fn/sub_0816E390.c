#define SceneMenuPanel_FinishMotionDelay sub_0816E390
#define SceneMenuPanel_UpdateMotion PauseMenu_UpdateSpriteMotion
#define SceneMenuPanel_StartPaletteTransition PauseMenu_StartPaletteTransition
#define SceneMenuPanel_GetPaletteBank sub_08165C4C
#define SceneMenuPanel_SetCallback Hud_SetInteractionCallback
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
u32 SceneMenuPanel_GetPaletteBank(struct SceneMenuPanel *);
void SceneMenuPanel_StartPaletteTransition(struct SceneMenuPanel *, u16, u32, s32);
void sub_0816D634(struct SceneMenuPanel *, u32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneMenuPanel *, void (*)(void), s32);
void PauseMenu_OnItemUseDelayUpdate(void);
void sub_0816E390(struct SceneMenuPanel *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->delay > 7) {
            u16 palette;
            panel->widgetFlags |= 1;
            sub_0816D634(panel, panel->selection);
            palette = 366;
            SceneMenuPanel_StartPaletteTransition(panel, palette, SceneMenuPanel_GetPaletteBank(panel), 16);
            Sound_PlaySong(269);
            SceneMenuPanel_SetCallback(panel, PauseMenu_OnItemUseDelayUpdate, 1);
        }
    }
}
