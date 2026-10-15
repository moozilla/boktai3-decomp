// MATCHING_CONCESSION: register-pinning
#include "scene_input_compare.h"
#define SceneInputPanel_ShowAttributeComparison sub_0805F63C
void SceneInputPanel_ShowAttributeComparison(struct SceneInputWidget *first,
                                             struct SceneInputWidget *second,
                                             struct SceneInputWidget *third,
                                             struct SceneInputResource *resource, s32 candidate)
{
    s32 digits[3];
    s32 current0, current1, current2, target2;
    register s32 target0 asm("r5");
    register s32 target1 asm("r6");
    s32 comparison;
    s32 i;
    register struct SceneInputWidget *widget asm("r5");
    current0 = sub_0815A384(0);
    current1 = sub_0815A384(1);
    current2 = sub_0815A384(2);
    target0 = sub_0815A3C4(0, candidate);
    target1 = sub_0815A3C4(1, candidate);
    target2 = sub_0815A3C4(2, candidate);
    comparison = 1;
    if (current0 >= target0) {
        comparison = 0;
        if (current0 > target0)
            comparison = 2;
    }
    {
        s32 *digit;
        s32 offset;
        sub_0805F374(target0, digits);
        {
            s32 product = comparison * 5;
            offset = product * 2;
        }
        asm("" : "+l"(offset));
        digit = digits;
        widget = first;
        i = 2;
        do {
            Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + offset), 1);
            widget++;
            i--;
        } while (i >= 0);
    }
    comparison = 1;
    if (current1 >= target1) {
        comparison = 0;
        if (current1 > target1)
            comparison = 2;
    }
    {
        s32 *digit;
        register s32 offset asm("r6");
        sub_0805F374(target1, digits);
        {
            register s32 product asm("r0") = comparison * 5;
            asm("" : "+r"(product));
            offset = product * 2;
        }
        digit = digits;
        widget = second;
        i = 2;
        do {
            Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + offset), 1);
            widget++;
            i--;
        } while (i >= 0);
    }
    comparison = 1;
    if (current2 >= target2) {
        comparison = 0;
        if (current2 > target2)
            comparison = 2;
    }
    {
        s32 *digit;
        register s32 offset asm("r6");
        sub_0805F374(target2, digits);
        asm("" : "+r"(comparison));
        {
            register s32 product asm("r0") = comparison * 5;
            asm("" : "+r"(product));
            offset = product * 2;
        }
        digit = digits;
        widget = third;
        i = 2;
        do {
            Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + offset), 1);
            widget++;
            i--;
        } while (i >= 0);
    }
}
