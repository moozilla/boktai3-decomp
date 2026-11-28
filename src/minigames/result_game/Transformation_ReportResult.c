#include "global.h"
#include "script/script_utility.h"
#include "minigames/result_game/result_tutorial.h"
#include "ui/town/shop_item_values.h"

static inline int clamp(int x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}
void Transformation_ReportResult(struct ResultMenu *menu)
{
    int values[7];
    struct ScriptUtilityArguments args;
    struct SceneValueEquipment *entry = Inventory_GetEquipmentEntry(menu->equipmentSlot);
    values[0] = clamp(menu->preciseCount);
    values[1] = clamp(menu->closeCount);
    values[2] = clamp(menu->missedCount);
    values[3] = menu->rankChange;
    values[4] = menu->gradeRaised;
    values[5] = menu->transformationOutcome;
    values[6] = entry->condition;
    args.count = 7;
    args.values = values;
    Script_Call(menu->completionScript, &args);
}
