#include "scene_input_compare.h"
#include "scene_hud_map.h"
#include "scene_input_grid.h"
void SceneInputPanel_ShowItemComparison(struct SceneInputWidget *primary,
                                        struct SceneInputWidget *secondary,
                                        struct SceneInputResource *resource,
                                        const struct SceneInputItemDescriptor *candidate,
                                        s32 referenceIndex)
{
    s32 digits[3];
    struct SceneInputItemDescriptor equipped;
    u32 converted[4];
    s32 primaryValue, secondaryValue;
    void *reference;
    s32 value, i, comparison;
    struct SceneInputWidget *widget;
    struct SceneInputMovingWidget *flags;
    primaryValue = 0;
    secondaryValue = 0;
    if (referenceIndex < 0)
        reference = 0;
    else
        reference = GameState_GetConsumableDescriptor(referenceIndex);
    SceneInputPanel_GetEquippedValues(&primaryValue, &secondaryValue, &equipped, 0);
    if (candidate) {
        Inventory_InitEquipmentEntryFromDescriptor(candidate, converted);
        value = Player_EvaluateItemStatA(converted, reference);
        comparison = 0;
        if (value > primaryValue)
            comparison = 1;
        else if (value < primaryValue)
            comparison = 2;
        SceneInputPanel_SplitThreeDigits(value, digits);
    } else {
        if (primaryValue > 0) {
            Inventory_InitEquipmentEntryFromDescriptor(&equipped, converted);
            value = Player_EvaluateItemStatA(converted, reference);
        } else
            value = 0;
        comparison = 0;
        if (value > primaryValue)
            comparison = 1;
        else if (value < primaryValue)
            comparison = 2;
        SceneInputPanel_SplitThreeDigits(value, digits);
    }
    if (primaryValue > 0 || candidate) {
        s32 offset;
        s32 *digit;
        SceneHud_FillTile001(primary->position.x >> 3, primary->position.y >> 3, 3);
        offset = comparison * 10;
        digit = digits;
        flags = (void *)&primary->flags;
        for (i = 2; i >= 0; i--) {
            flags->flags &= ~1;
            Anim_LoadKeyframe(primary, resource, (u16)(*digit++ + offset), 1);
            flags++;
            primary++;
        }
    } else {
        struct SceneInputMovingWidget *hidden;
        u32 mask;
        SceneHud_FillTile020(primary->position.x >> 3, primary->position.y >> 3, 3);
        mask = 1;
        hidden = (void *)&primary->flags;
        for (i = 2; i >= 0; i--) {
            hidden->flags |= mask;
            hidden++;
        }
    }
    SceneInputPanel_SplitThreeDigits(secondaryValue, digits);
    if (secondaryValue > 0 && !candidate) {
        s32 offset;
        s32 *digit;
        SceneHud_FillTile001(secondary->position.x >> 3, secondary->position.y >> 3, 3);
        offset = 0;
        digit = digits;
        flags = (void *)&secondary->flags;
        widget = secondary;
        for (i = 2; i >= 0; i--) {
            flags->flags &= ~1;
            Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + offset), 1);
            flags++;
            widget++;
        }
    } else {
        struct SceneInputMovingWidget *hidden;
        u32 mask;
        SceneHud_FillTile020(secondary->position.x >> 3, secondary->position.y >> 3, 3);
        mask = 1;
        widget = secondary;
        hidden = (void *)&widget->flags;
        for (i = 2; i >= 0; i--) {
            hidden->flags |= mask;
            hidden++;
        }
    }
}
