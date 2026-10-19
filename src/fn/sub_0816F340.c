#include "global.h"
#include "scene_hud_display.h"
#ifdef NONMATCHING
// fuzzy: 96.6%
// foundry draft (worker best 96.6%); what differs is mostly register allocation and instruction order
#define SceneHud_AnimateActionResult sub_0816F340
#define SceneHud_TickWidgetMotion PauseMenu_UpdateSpriteMotion
#define SceneHud_BeginPaletteBlend PauseMenu_StartPaletteTransition
#define SceneHud_SetWidgetPalette PauseMenu_SetSpritePalette
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_StartPaletteTransition(void *, u32, u32, s32);
u32 PauseMenu_UpdateSpriteMotion(void *, u32);
u32 PlayerPanel_GetStatusValue(void *);
void sub_0816F3FC(void);
void sub_0816F340(struct SceneHudState *s)
{
    u8 index;
    s32 mode;
    u32 palette;
    struct SceneInputWidget *widget;
    if (!PauseMenu_UpdateSpriteMotion(s, 0))
        return;
    if (++s->actionTransferDelay <= 7)
        return;
    s->motionWidgets[0].flags |= 1;
    {
        u16 basePalette = 0x16E;
        PauseMenu_StartPaletteTransition(s, basePalette, PlayerPanel_GetStatusValue(s), 16);
    }
    if (!s->actionTransferBonus) {
        index = 13;
        mode = 2;
        palette = 0x3A8;
    } else {
        index = 13;
        mode = 1;
        palette = 0x3A9;
    }
    widget = &s->motionWidgets[14];
    Anim_SetKeyframes(widget, s->actionResultResources[s->actionTransferBonus], index, mode, 4);
    PauseMenu_SetSpritePalette(widget, palette);
    Sound_PlaySong(0x10D);
    Hud_SetInteractionCallback(s, sub_0816F3FC, 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816F340);
#endif
