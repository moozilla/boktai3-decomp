#include "global.h"
#include "ui/town/shop_item_values.h"
#include "engine/ram.h"
#include "minigames/result_game/result_tutorial.h"
#include "ui/town/shop_item_values.h"

extern struct SceneValueEquipmentDefinition gUnk_08E6092C[];
extern struct SceneValueConsumableDefinition gUnk_08E60F80[];
extern u16 gUnk_086141BC[];
void sub_0801B600(u32 equipmentSlot, u32 consumableSlot, struct SceneValueEquipment *result);
void TransformMarker_CopyVector(struct SceneValueEquipment *entry,
                                const struct SceneValueEquipmentDefinition *definition);
void TransformMarker_ClearVector(struct SceneValueEquipment *entry);
void Inventory_MarkEquipmentObtained(u32);
void GameState_ClearConsumableSlot(u32);
s32 Div(s32, s32);

void sub_0801B700(struct ResultMenu *menu)
{
    struct SceneValueEquipment *entry = Inventory_GetEquipmentEntry(menu->equipmentSlot);
    struct SceneValueConsumableDefinition *item =
        &gUnk_08E60F80[Inventory_GetConsumableId(menu->consumableSlot)];
    struct SceneValueEquipmentDefinition *oldDefinition, *newDefinition;
    s32 chance;
    s32 tracking, press, score;
    u32 kind;
    sub_0801B600(menu->equipmentSlot, menu->consumableSlot, &menu->transformationResult);
    oldDefinition = &gUnk_08E6092C[entry->kind];
    newDefinition = &gUnk_08E6092C[menu->transformationResult.kind];
    menu->rankChange = newDefinition->descriptionStyleA - oldDefinition->descriptionStyleA;
    menu->gradeRaised = menu->transformationResult.grade > entry->grade;
    switch (item->unknownA[0]) {
        case 20:
            chance = 0x1999;
            break;
        case 30:
            chance = 0x2666;
            break;
        case 40:
            chance = 0x3333;
            break;
        case 50:
            chance = 0x4000;
            break;
        case 60:
            chance = 0x8000;
            break;
        case 0xff:
            chance = 0x10000;
            break;
        case 10:
        default:
            chance = 0xccc;
            break;
    }
    if (menu->wideTimingWindow)
        chance <<= 1;
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    if (gUnk_086141BC[gEffectRandomTableIndex] < chance) {
        TransformMarker_CopyVector(&menu->transformationResult, newDefinition);
    } else {
        if (entry->kind != menu->transformationResult.kind)
            TransformMarker_ClearVector(&menu->transformationResult);
    }
    if (newDefinition->modifierTextA == 0 && newDefinition->modifierTextB == 0)
        menu->transformationOutcome = 0;
    else if (entry->kind == menu->transformationResult.kind) {
        if (entry->modifierA || entry->modifierB)
            menu->transformationOutcome = 2;
        else if (menu->transformationResult.modifierA || menu->transformationResult.modifierB)
            menu->transformationOutcome = 3;
        else
            menu->transformationOutcome = 1;
    } else if (menu->transformationResult.modifierA || menu->transformationResult.modifierB)
        menu->transformationOutcome = 3;
    else
        menu->transformationOutcome = 1;
    tracking = Div(menu->trackingScore << 8, menu->trackingSamples);
    press = Div(menu->pressScoreTotal << 8, menu->pressSamples);
    score = (tracking * 0x66 + press * 0x99) >> 8;
    if (score > 0x400)
        menu->transformationResult.condition = 1000;
    else if (score > 0x300)
        menu->transformationResult.condition = 750;
    else if (score > 0x200)
        menu->transformationResult.condition = 500;
    else
        menu->transformationResult.condition = 250;
    *entry = menu->transformationResult;
    Inventory_MarkEquipmentObtained(menu->transformationResult.kind);
    kind = menu->transformationResult.kind;
    if ((s32)kind < 0x20)
        gGameState->unlockedEquipment[0] |= 1 << kind;
    else
        gGameState->unlockedEquipment[1] |= 1 << (kind - 0x20);
    GameState_ClearConsumableSlot(menu->consumableSlot);
}
