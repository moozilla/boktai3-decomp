#include "scene_hud_display.h"
/* SceneHud_UpdateFeaturePalettes: Sets up to three widget palette banks to 0x374 or 0x373 according to owner flag masks 4/8/16; preserves the existing provisional alias */
#define SceneHud_UpdateFeaturePalettes sub_0805E014
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
