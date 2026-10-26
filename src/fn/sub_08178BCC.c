#define gSceneHud gFieldHud
#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
extern struct SceneHudState *gSceneHud;
extern u16 gSceneHudMode;
void SceneHud_RemoveAnimations(struct SceneHudState *);
void PauseMenu_RemoveHeaderAnimation(struct SceneHudState *);
void PlayerPanel_RemoveEffects(struct SceneHudState *);
void Hud_RemoveMenuGraphics(struct SceneHudState *);
void PauseMenu_RemoveEquipmentSlotAnimations(struct SceneHudState *);
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 OwnerWidgetsEnabled(u32 mask)
{
    if ((gGameState->flags868 & mask))
        return 1;
    return 0;
}
s32 SceneHud_DestroyOwner(struct SceneHudState *s)
{
    struct SceneInputWidget *widget;
    s32 count;
    SceneHud_RemoveAnimations(s);
    PauseMenu_RemoveHeaderAnimation(s);
    PlayerPanel_RemoveEffects(s);
    Hud_RemoveMenuGraphics(s);
    PauseMenu_RemoveEquipmentSlotAnimations(s);
    if (OwnerWidgetsEnabled(0x80) && s->initializationFlag) {
        widget = s->ownerWidgets;
        count = 1;
        do {
            if (widget->unknown4[0])
                Anim_Remove(widget);
            widget++;
        } while (--count >= 0);
        widget = s->ownerAuxWidgets;
        count = 5;
        do {
            if (widget->unknown4[0])
                Anim_Remove(widget);
            widget++;
        } while (--count >= 0);
    }
    gSceneHudMode = s->mode;
    gSceneHud = 0;
    return 0;
}
