#include "scene_panel_rig.h"
static inline s32 scale12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline s32 absolute(s32 v)
{
    return v < 0 ? -v : v;
}
void ScenePanelRig_TickPrimarySprites(struct ScenePanelRig *rig, u32 palette)
{
    s32 i;
    for (i = 0;;) {
        struct SceneRigSprite *sprite = &rig->primary[i];
        struct SceneRigSprite *output = sprite;
        s32 radius;
        radius =
            scale12(absolute(gUnk_086149C4[((sprite->randomPhase & 255) + 64) & 255]) * 40) + 60;
        output->position.x =
            rig->position.x + scale12(gUnk_086149C4[(sprite->phase + 64) & 255] * radius);
        output->position.y = rig->position.y;
        output->position.z = rig->position.z + scale12(gUnk_086149C4[sprite->phase & 255] * radius);
        if (rig->state == 2)
            sprite->randomPhase += 4;
        else
            sprite->randomPhase += 2;
        if (i & 1)
            sprite->phase = (sprite->phase + 1) & 255;
        else
            sprite->phase = (sprite->phase + 255) & 255;
        ScenePanelRig_StepSprite(sprite);
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        {
            u32 step = 1;
            i += step;
        }
        {
            s32 compare = i;
            if (compare > 7)
                break;
        }
    }
}
