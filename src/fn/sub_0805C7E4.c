#include "scene_panel_commands.h"
void ScenePanel_UpdateApproachSprite(struct ScenePanelInteractionView *panel)
{
    if (panel->mode && panel->activeCount && (panel->state == 10 || panel->state == 14)) {
        u32 scale, offset;
        struct ScenePanelApproachSprite *sprite = &panel->approachSprite;
        sprite->flags &= ~1;
        sprite->frame = 1;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        offset = gUnk_03005308 * 2;
        scale = *(const u16 *)(offset + (u32)gUnk_0203B400);
        scale &= 15;
        scale += 72;
        panel->approachSprite.scaleY = scale;
        panel->approachSprite.scaleX = scale;
    } else
        panel->approachSprite.flags |= 1;
}
