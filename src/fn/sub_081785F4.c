#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#define SceneHud_RefreshActionKeyframes sub_081785F4
void sub_081785F4(struct SceneHudState *hud)
{
    if (hud->actionOwner->actionSelectionStamp && (++hud->actionTransferDelay & 1))
        Anim_LoadKeyframe(&hud->primaryWidgets[0], hud->motionResource, 0xC9, 1);
    else
        Anim_LoadKeyframe(&hud->primaryWidgets[0], hud->motionResource,
                          (u16)((s8)hud->actionOwner->thresholdMode + 0x184), 1);
    Anim_LoadKeyframe(&hud->secondaryWidgets[0], hud->motionResource,
                      hud->actionOwner->statModifierContext.actionKind, 1);
}
