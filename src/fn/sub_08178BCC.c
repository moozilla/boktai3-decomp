#define gSceneHud gUnk_02000214
#define gSceneHudMode gUnk_020005A4
#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
extern struct SceneHudState *gSceneHud;
extern u16 gSceneHudMode;
void sub_08169738(struct SceneHudState *);
void PauseMenu_RemoveHeaderAnimation(struct SceneHudState *);
void sub_08167734(struct SceneHudState *);
void Hud_RemoveMenuGraphics(struct SceneHudState *);
void PauseMenu_RemoveEquipmentSlotAnimations(struct SceneHudState *);
static inline u32 TestFlags(u32 *flags, u32 mask) { return *flags & mask; }
static inline u8 OwnerWidgetsEnabled(u32 mask)
{
    if ((gUnk_02000710->flags868 & mask)) return 1;
    return 0;
}
s32 SceneHud_DestroyOwner(struct SceneHudState *s)
{
    struct SceneInputWidget *widget;
    s32 count;
    sub_08169738(s);
    PauseMenu_RemoveHeaderAnimation(s);
    sub_08167734(s);
    Hud_RemoveMenuGraphics(s);
    PauseMenu_RemoveEquipmentSlotAnimations(s);
    if (OwnerWidgetsEnabled(0x80) && s->initializationFlag) {
        widget = s->ownerWidgets;
        count = 1;
        do {
            if (widget->unknown4[0]) Anim_Remove(widget);
            widget++;
        } while (--count >= 0);
        widget = s->ownerAuxWidgets;
        count = 5;
        do {
            if (widget->unknown4[0]) Anim_Remove(widget);
            widget++;
        } while (--count >= 0);
    }
    gSceneHudMode = s->mode;
    gSceneHud = 0;
    return 0;
}
