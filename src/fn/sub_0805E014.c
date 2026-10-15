#include "scene_hud_display.h"
#define SceneHud_UpdateFeaturePalettes sub_0805E014
void SceneHud_UpdateFeaturePalettes(const void *owner, struct SceneInputWidget *first,
                                    struct SceneInputWidget *second, struct SceneInputWidget *third)
{
    if (first) {
        if (sub_0805E7D0(owner, 4))
            sub_0805E000(first, 0x374);
        else
            sub_0805E000(first, 0x373);
    }
    if (second) {
        if (sub_0805E7D0(owner, 8))
            sub_0805E000(second, 0x374);
        else
            sub_0805E000(second, 0x373);
    }
    if (third) {
        if (sub_0805E7D0(owner, 16))
            sub_0805E000(third, 0x374);
        else
            sub_0805E000(third, 0x373);
    }
}
