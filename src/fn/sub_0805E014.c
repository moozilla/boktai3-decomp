#include "scene_hud_display.h"
#define SceneHud_UpdateFeaturePalettes sub_0805E014
void SceneHud_UpdateFeaturePalettes(const void *owner, struct SceneInputWidget *first,
                                    struct SceneInputWidget *second, struct SceneInputWidget *third)
{
    if (first) {
        if (SceneInput_TestGridFlags(owner, 4))
            SceneHud_SetKeyframePaletteBank(first, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(first, 0x373);
    }
    if (second) {
        if (SceneInput_TestGridFlags(owner, 8))
            SceneHud_SetKeyframePaletteBank(second, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(second, 0x373);
    }
    if (third) {
        if (SceneInput_TestGridFlags(owner, 16))
            SceneHud_SetKeyframePaletteBank(third, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(third, 0x373);
    }
}
