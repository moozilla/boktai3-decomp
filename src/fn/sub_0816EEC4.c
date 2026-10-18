#define SceneMenuPanel_FinishResultMotion sub_0816EEC4
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
    u8 unknown2AAC[0x1DB5];
    u8 result;
};
u32 SceneMenuPanel_UpdateMotion(struct SceneMenuPanel *, u32);
u32 SceneMenuPanel_GetPaletteBank(struct SceneMenuPanel *);
void SceneMenuPanel_StartPaletteTransition(struct SceneMenuPanel *, u16, u32, s32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneMenuPanel *, void (*)(void), s32);
void sub_0816F0E8(void);
void sub_0816EF4C(void);
void sub_0816EEC4(struct SceneMenuPanel *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->delay > 7) {
            u16 palette;
            panel->widgetFlags |= 1;
            palette = 366;
            SceneMenuPanel_StartPaletteTransition(panel, palette, SceneMenuPanel_GetPaletteBank(panel), 16);
            Sound_PlaySong(269);
            if (panel->result == 0)
                SceneMenuPanel_SetCallback(panel, sub_0816F0E8, 1);
            else
                SceneMenuPanel_SetCallback(panel, sub_0816EF4C, 1);
        }
    }
}
