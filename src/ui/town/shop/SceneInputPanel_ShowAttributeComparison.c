#include "ui/widgets/scene_input_compare.h"
void SceneInputPanel_ShowAttributeComparison(struct AnimSprite *first, struct AnimSprite *second,
                                             struct AnimSprite *third,
                                             struct AnimSpriteResource *resource, s32 candidate)
{
    s32 digits[3];
    s32 current0, current1, current2, target0, target1, target2;
    s32 comparison;
    s32 i;
    current0 = Player_GetCurrentAttribute(0);
    current1 = Player_GetCurrentAttribute(1);
    current2 = Player_GetCurrentAttribute(2);
    target0 = Player_GetCandidateAttribute(0, candidate);
    target1 = Player_GetCandidateAttribute(1, candidate);
    target2 = Player_GetCandidateAttribute(2, candidate);
    comparison = 1;
    if (current0 >= target0) {
        comparison = 0;
        if (current0 > target0)
            comparison = 2;
    }
    SceneInputPanel_SplitThreeDigits(target0, digits);
    for (i = 0; i < 3; i++)
        Anim_LoadKeyframe(&first[i], resource, (u16)(digits[i] + comparison * 10), 1);
    comparison = 1;
    if (current1 >= target1) {
        comparison = 0;
        if (current1 > target1)
            comparison = 2;
    }
    SceneInputPanel_SplitThreeDigits(target1, digits);
    for (i = 0; i < 3; i++)
        Anim_LoadKeyframe(&second[i], resource, (u16)(digits[i] + comparison * 10), 1);
    comparison = 1;
    if (current2 >= target2) {
        comparison = 0;
        if (current2 > target2)
            comparison = 2;
    }
    SceneInputPanel_SplitThreeDigits(target2, digits);
    for (i = 0; i < 3; i++)
        Anim_LoadKeyframe(&third[i], resource, (u16)(digits[i] + comparison * 10), 1);
}
