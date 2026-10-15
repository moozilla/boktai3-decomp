// MATCHING_CONCESSION: register-pinning
#include "scene_input_compare.h"
#include "scene_hud_map.h"
#include "scene_input_grid.h"
#define SceneInputPanel_ShowItemComparison sub_0805F480
void SceneInputPanel_ShowItemComparison(struct SceneInputWidget *primary,
                                        struct SceneInputWidget *secondary,
                                        struct SceneInputResource *resource,
                                        const struct SceneInputItemDescriptor *candidateInput,
                                        s32 referenceInput)
{
    register const struct SceneInputItemDescriptor *candidate asm("r10") = candidateInput;
    s32 referenceIndex = referenceInput;
    s32 digits[3];
    struct SceneInputItemDescriptor equipped;
    u32 converted[4];
    s32 primaryValue, secondaryValue;
    void *reference;
    s32 value, offset, i;
    s32 comparison;
    asm("" : "+r"(referenceIndex));
    primaryValue = 0;
    secondaryValue = 0;
    if (referenceIndex < 0)
        reference = 0;
    else
        reference = sub_0822E288(referenceIndex);
    sub_0805F3A4(&primaryValue, &secondaryValue, &equipped, 0);
    if (candidate) {
        sub_0822D740(candidate, converted);
        value = sub_08159030(converted, reference);
        comparison = 0;
        if (value > primaryValue)
            comparison = 1;
        else if (value < primaryValue)
            comparison = 2;
        sub_0805F374(value, digits);
    } else {
        if (primaryValue > 0) {
            sub_0822D740(&equipped, converted);
            value = sub_08159030(converted, reference);
        } else
            value = 0;
        comparison = 0;
        if (value > primaryValue)
            comparison = 1;
        else if (value < primaryValue)
            comparison = 2;
        sub_0805F374(value, digits);
    }
    {
        if (primaryValue > 0 || ({
                register const void *test asm("r1") = candidate;
                asm("" : "+r"(test));
                test;
            })) {
            s32 *digit;
            struct SceneInputMovingWidget *flags;
            SceneHud_FillTile001(primary->position.x >> 3, primary->position.y >> 3, 3);
            offset = comparison * 10;
            digit = digits;
            flags = (void *)&primary->flags;
            asm("" : "+r"(flags));
            for (i = 2; i >= 0; i--) {
                flags->flags &= ~1;
                Anim_LoadKeyframe(primary, resource, (u16)(*digit++ + offset), 1);
                flags++;
                primary++;
            }
        } else {
            struct SceneInputMovingWidget *flags;
            u32 hide;
            SceneHud_FillTile020(primary->position.x >> 3, primary->position.y >> 3, 3);
            hide = 1;
            flags = (void *)&primary->flags;
            for (i = 2; i >= 0; i--) {
                flags->flags |= hide;
                flags++;
            }
        }
    }
    sub_0805F374(secondaryValue, digits);
    {
        if (secondaryValue > 0 && !({
                register const void *test asm("r1") = candidate;
                asm("" : "+r"(test));
                test;
            })) {
            s32 *digit;
            struct SceneInputWidget *widget;
            struct SceneInputMovingWidget *flags;
            register u32 zero asm("r9");
            u32 temp;
            register struct SceneInputWidget *copy asm("r4");
            {
                register struct SceneInputWidget *measure asm("r2") = secondary;
                SceneHud_FillTile001(measure->position.x >> 3, measure->position.y >> 3, 3);
            }
            temp = 0;
            zero = temp;
            digit = digits;
            copy = secondary;
            flags = (void *)&copy->flags;
            widget = secondary;
            for (i = 2; i >= 0; i--) {
                flags->flags &= ~1;
                Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + zero), 1);
                flags++;
                widget++;
            }
        } else {
            struct SceneInputMovingWidget *flags;
            u32 hide;
            struct SceneInputWidget *copy;
            {
                struct SceneInputWidget *measure = secondary;
                asm("" : "+r"(measure));
                SceneHud_FillTile020(measure->position.x >> 3, measure->position.y >> 3, 3);
            }
            hide = 1;
            copy = secondary;
            flags = (void *)&copy->flags;
            for (i = 2; i >= 0; i--) {
                flags->flags |= hide;
                flags++;
            }
        }
    }
}
