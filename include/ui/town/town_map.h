#ifndef GUARD_REGISTRY_CHOICE_MENU_H
#define GUARD_REGISTRY_CHOICE_MENU_H
#include "ui/widgets/menu_cursor.h"

/* Registry script 3F74: eleven choices, with a separate script for choice one. */
struct RegistryChoiceMenu {
    struct Task task;
    void *background;
    u16 *palettes;
    struct AnimSprite animations[5];
    struct AnimSpriteResource resource;
    u32 unknown220;
    s32 initialSelection, selection;
    struct MenuCursor *cursor;
    s32 entryScripts[11];
    s32 alternateScript;
    void (*update)(struct RegistryChoiceMenu *);
    s32 timer;
};
void TownMap_SetState(struct RegistryChoiceMenu *, void (*)(struct RegistryChoiceMenu *));
void TownMap_ApplyPaletteStep(struct RegistryChoiceMenu *, u32);
void TownMap_UpdateAnimations(struct RegistryChoiceMenu *);
void TownMap_ConfirmSelection(struct RegistryChoiceMenu *);
void TownMap_InitAnimations(struct RegistryChoiceMenu *);
void TownMap_InitBackground(struct RegistryChoiceMenu *);
void TownMap_ConfigureDisplay(void);
void TownMap_UpdateSelection(struct RegistryChoiceMenu *);
void TownMap_UpdateOpeningDelay(struct RegistryChoiceMenu *);
void TownMap_UpdateScriptDelay(struct RegistryChoiceMenu *);
void TownMap_StartSelection(struct RegistryChoiceMenu *);
void TownMap_StartOpeningFade(struct RegistryChoiceMenu *);
void TownMap_StartClosingFade(struct RegistryChoiceMenu *);
void TownMap_OnUpdate(struct RegistryChoiceMenu *);
void TownMap_OnDestroy(struct RegistryChoiceMenu *);
void TownMap_CreateSelectionCursor(struct RegistryChoiceMenu *, s32);
s32 TownMap_InitFromScript(struct RegistryChoiceMenu *, void *);
struct RegistryChoiceMenu *TownMap_Create(void *);
#endif
